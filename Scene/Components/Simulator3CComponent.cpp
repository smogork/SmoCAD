//
// Created by ksm on 10/23/22.
//

#include "Simulator3CComponent.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Simulator3CSystem.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"
#include "Loaders/GCodeLoader.h"
#include "Scene/Entities/Simulator/SimulationProcess.h"
#include "Controls/EntityContextMenu.h"

#include <cmath>
#include <QMessageBox>

std::shared_ptr<Simulator3CComponent>
Simulator3CComponent::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<Simulator3CSystem>().lock();
        auto item = system->CreateRegistered(oid, trans);
        return item;
    }
    return nullptr;
}

Simulator3CComponent::Simulator3CComponent(unsigned int oid) : Simulator3CComponent(oid, nullptr) {}

void Simulator3CComponent::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<Simulator3CSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Simulator3CComponent::Simulator3CComponent(unsigned int oid, std::shared_ptr<Transform> simulatorTransform)
        : IComponent(oid, SIMULATOR3C), p_Transform(simulatorTransform), m_state(IDLE)
{
    m_heightMap = std::make_shared<QImageOperator>(m_blockParams.TextureWidthX, m_blockParams.TextureWidthY);
    InitializeHeightMap();
    
    m_woodTexture = std::make_shared<QOpenGLTexture>(QImage("Resources/wood.png"),
                                                     QOpenGLTexture::MipMapGeneration::GenerateMipMaps);
    
    ResizeBlock();
    m_cutter = std::make_unique<CutterObject>(QVector3D(), m_cutterParams, p_Transform);
    
    ChangeCutterType(m_cutterParams.Type);
    m_state = IDLE;
    MoveCutterToIdleState();
}

Simulator3CComponent::~Simulator3CComponent()
{
    if (m_simProcess)
    {
        emit AbortSimulation();
        m_simProcess->quit();
        m_simProcess->wait();
    }
    
    UnregisterComponent();
}

void Simulator3CComponent::InitializeHeightMap()
{
    m_heightMap->ChangeSize(m_blockParams.TextureWidthX, m_blockParams.TextureWidthY);
    //m_heightMap.GetRedColorFunction = ASSIGN_COLOR_FUNCTION(&Simulator3CComponent::CutterColorFunction);
}

void Simulator3CComponent::LoadPathFile(const QString &filepath)
{
    if (!CanLoadPaths())
        return;
    
    try
    {
        std::unique_ptr<CutterPath> cutterPath = GCodeLoader::LoadCutterPath(filepath);
        ChangeCutterParameters(cutterPath->Cutter);
        m_pathPolyline = std::make_unique<CutterPathPolyline>(cutterPath->Points, p_Transform);
        m_cutter->p_Transform->Position = cutterPath->Points[0];
        SimulationProgress = 0;
        
        if (m_simProcess)
        {
            emit AbortSimulation();
            m_simProcess->quit();
            m_simProcess->wait();
        }
        
        m_simProcess = std::make_unique<SimulationProcess>(std::move(cutterPath), m_heightMap, m_blockParams);
        connect(m_simProcess.get(), &SimulationProcess::finished, m_simProcess.get(), &QObject::deleteLater);
        connect(this, &Simulator3CComponent::PlaySimulation, m_simProcess.get(), &SimulationProcess::onPlaySimulation);
        connect(this, &Simulator3CComponent::PauseSimulation, m_simProcess.get(), &SimulationProcess::onPauseSimulation);
        connect(this, &Simulator3CComponent::SkipSimulation, m_simProcess.get(), &SimulationProcess::onSkipSimulation);
        connect(this, &Simulator3CComponent::AbortSimulation, m_simProcess.get(), &SimulationProcess::onAbortSimulation);
        
        connect(m_simProcess.get(), &SimulationProcess::SimulationResultReady, this,
                &Simulator3CComponent::onSimulationResultsHandle);
        connect(m_simProcess.get(), &SimulationProcess::SimulationError, this, &Simulator3CComponent::onSimulationError);
        connect(m_simProcess.get(), &SimulationProcess::SimulationProgress, this, &Simulator3CComponent::onSimulationProgress);
        connect(m_simProcess.get(), &SimulationProcess::SimulationFinished, this, &Simulator3CComponent::onSimulationFinished);
        m_simProcess->start();
        m_state = PAUSED;
    }
    catch (std::runtime_error &e)
    {
        qDebug() << "Error on loading paths " << e.what();
        
        MoveCutterToIdleState();
        m_state = IDLE;
        throw std::runtime_error(
                QString("Error on loading paths from %0.\n%1").arg(filepath).arg(e.what()).toStdString());
    }
}

void Simulator3CComponent::MoveCutterToIdleState()
{
    m_cutter->p_Transform->Position = QVector3D(
            -m_blockParams.WidthX.GetSceneUnits() / 2.0f,
            m_blockParams.Height.GetSceneUnits() + 2.0f,
            -m_blockParams.WidthY.GetSceneUnits() / 2.0f);
}

void Simulator3CComponent::ChangeCutterType(CutterType type)
{
    if (!CanChangeCutterParams())
        return;
    
    m_cutterParams.Type = type;
    ChangeCutterParameters(m_cutterParams);
}

void Simulator3CComponent::ChangeCutterDiameter(Length diameter)
{
    if (!CanChangeCutterParams())
        return;
    
    m_cutterParams.Diameter = diameter;
    ChangeCutterParameters(m_cutterParams);
}

bool Simulator3CComponent::CanChangeCutterParams()
{
    return m_state == PAUSED || m_state == IDLE;
}

bool Simulator3CComponent::CanLoadPaths()
{
    return m_state == PAUSED || m_state == IDLE;
}

CutterParameters Simulator3CComponent::GetCutterParameters()
{
    return m_cutterParams;
}

BlockParameters Simulator3CComponent::GetBlockParameters()
{
    return m_blockParams;
}

float Simulator3CComponent::CutterColorFunction(int x, int y, QVector3D startCutterPos, QVector3D endCutterPos)
{
    QVector2D simPoint = TextureToSim(x, y);
    
    float distToMoveLine = DistToSegment(simPoint,
                                         QVector2D(startCutterPos.x(), startCutterPos.z()),
                                         QVector2D(endCutterPos.x(), endCutterPos.z()));
    
    float r = m_cutterParams.Diameter.GetSceneUnits() / 2;
    if (distToMoveLine > r)
        return NAN;
    
    switch (m_cutterParams.Type)
    {
        case Cylindrical:
            return CutterHeightToTextureColor(0, startCutterPos.y(), endCutterPos.y());
        case Spherical:
            float cHeight = r - std::sqrt(r * r - distToMoveLine * distToMoveLine);
            return CutterHeightToTextureColor(cHeight, startCutterPos.y(), endCutterPos.y());
    }
    return NAN;
}

QVector2D Simulator3CComponent::TextureToCutter(int texX, int texY, QVector3D CutterSimPos)
{
    return QVector2D(
            ((float) texX / m_blockParams.TextureWidthX - 0.5f) * m_blockParams.WidthX.GetSceneUnits() -
            CutterSimPos.x(),
            ((float) texY / m_blockParams.TextureWidthY - 0.5f) * m_blockParams.WidthY.GetSceneUnits() -
            CutterSimPos.z()
    );
}

float Simulator3CComponent::CutterHeightToTextureColor(float cutterHeight, float startHeight, float finishHeight)
{
    if (startHeight - finishHeight < 1e-4)
    {
        float red = cutterHeight + startHeight;
        return red / m_blockParams.Height.GetSceneUnits();
    }
    
    //TODO: Stworzenie gradientu
    return 1;
}

QVector2D Simulator3CComponent::TextureToSim(int texX, int texY)
{
    return QVector2D(
            ((float) texX / m_blockParams.TextureWidthX - 0.5f) * m_blockParams.WidthX.GetSceneUnits(),
            ((float) texY / m_blockParams.TextureWidthY - 0.5f) * m_blockParams.WidthY.GetSceneUnits()
    );
}

float Simulator3CComponent::DistToSegment(QVector2D p, QVector2D A, QVector2D B)
{
    QVector2D v = B - A;
    QVector2D vp = p - A;
    float lengthAb = v.length();
    
    v.normalize();
    float scale = QVector2D::dotProduct(v, vp.normalized()) * vp.length();
    
    QVector2D thrownP = A + std::clamp(scale, 0.0f, lengthAb) * v;
    return (p - thrownP).length();
}

void Simulator3CComponent::SkipPathToEnd()
{
    if (m_state == IDLE)
        return;
    
    emit SkipSimulation();
    m_state = MILLING;
}

QPoint Simulator3CComponent::GetCutterTextureRadius()
{
    float r = m_cutterParams.Diameter.GetSceneUnits() / 2.0f;
    return QPoint(r / m_blockParams.WidthX.GetSceneUnits() * m_blockParams.TextureWidthX,
                  r / m_blockParams.WidthY.GetSceneUnits() * m_blockParams.TextureWidthY);
}

void Simulator3CComponent::ChangeCutterParameters(CutterParameters params)
{
    m_cutterParams = params;
    m_cutter->SetCutterParameters(m_cutterParams);
    
    switch (m_cutterParams.Type)
    {
        case Cylindrical:
            m_heightMap->PrepareCylindricalStamp(GetCutterTextureRadius().x(), GetCutterTextureRadius().y(),
                                                 m_cutterParams.Diameter.GetSceneUnits() / 2.0f);
            break;
        case Spherical:
            m_heightMap->PrepareSphericalStamp(GetCutterTextureRadius().x(), GetCutterTextureRadius().y(),
                                               m_cutterParams.Diameter.GetSceneUnits() / 2.0f);
            break;
    }
}

void Simulator3CComponent::ChangeToolSubmersions(Length toolSub, Length globalSub)
{
    if (m_state == MILLING)
        return;
    
    m_heightMap->MaximalGlobalSubmerison = globalSub.GetSceneUnits();
    m_heightMap->MaximumToolSubmersion = toolSub.GetSceneUnits();
}

void Simulator3CComponent::ChangeBlockSize(Length X, Length Y, Length Height)
{
    m_blockParams.WidthX = X;
    m_blockParams.WidthY = Y;
    m_blockParams.Height = Height;
    
    ResizeBlock();
    ChangeCutterParameters(m_cutterParams);
}

void Simulator3CComponent::ChangeBlockVertices(int X, int Y)
{
    m_blockParams.VertexWidthX = X;
    m_blockParams.VertexWidthY = Y;
    
    ResizeBlock();
    ChangeCutterParameters(m_cutterParams);
}

void Simulator3CComponent::ChangeTextureSize(int size)
{
    m_blockParams.TextureWidthX = m_blockParams.TextureWidthY = size;
    InitializeHeightMap();
    ChangeCutterParameters(m_cutterParams);
}

void Simulator3CComponent::ResizeBlock()
{
    m_blockLower = std::make_unique<BlockLowerWall>(QVector3D(), p_Transform, m_blockParams.WidthX.GetSceneUnits(),
                                                    m_blockParams.WidthY.GetSceneUnits());
    m_blockSide = std::make_unique<BlockSideWall>(QVector3D(), p_Transform, m_heightMap->GetTexture(),
                                                  m_blockParams.WidthX.GetSceneUnits(),
                                                  m_blockParams.WidthY.GetSceneUnits(),
                                                  m_blockParams.Height.GetSceneUnits(), m_blockParams.VertexWidthX,
                                                  m_blockParams.VertexWidthY);
    m_blockUpper = std::make_unique<BlockUpperWall>(QVector3D(0, m_blockParams.Height.GetSceneUnits(), 0), p_Transform,
                                                    m_heightMap->GetTexture(), m_woodTexture,
                                                    m_blockParams.WidthX.GetSceneUnits(),
                                                    m_blockParams.WidthY.GetSceneUnits(),
                                                    m_blockParams.Height.GetSceneUnits(),
                                                    m_blockParams.VertexWidthX, m_blockParams.VertexWidthY);
}

void Simulator3CComponent::HidePathsOnScene(bool hide)
{
    if (m_state == IDLE)
        return;
    
    m_pathPolyline->p_Drawing->Enabled = hide;
}

std::tuple<Length, Length> Simulator3CComponent::GetToolSubmersions()
{
    return {Length::FromCentimeters(m_heightMap->MaximumToolSubmersion),
            Length::FromCentimeters(m_heightMap->MaximalGlobalSubmerison)};
}

bool Simulator3CComponent::GetPathsHide()
{
    if (m_state == IDLE)
        return false;
    
    if (m_pathPolyline)
        return m_pathPolyline->p_Drawing->Enabled;
    return false;
}

void Simulator3CComponent::PlayPauseSimulation()
{
    //emit PauseSimulation();
    if (m_state == PAUSED)
    {
        emit PlaySimulation();
        m_state = MILLING;
    } else if (m_state == MILLING)
    {
        emit PauseSimulation();
        m_state = PAUSED;
    }
}

void Simulator3CComponent::onSimulationResultsHandle(QVector3D cutterSimPos)
{
    m_heightMap->UploadToGPU();
    m_cutter->p_Transform->Position = cutterSimPos;
    EntityContextMenu::MakeRepaint();
}

void Simulator3CComponent::onSimulationError(std::string msg)
{
    emit AbortSimulation();
    static std::string lastError;
    if (msg.compare(lastError) == 0)
        return;
    lastError = msg;
    

    QMessageBox msgBox;
    msgBox.setText(QString(msg.c_str()));
    msgBox.exec();
}

void Simulator3CComponent::onSimulationProgress(int progress)
{
    SimulationProgress = progress;
}

void Simulator3CComponent::onSimulationFinished(QVector3D cutterSimPos)
{
    onSimulationResultsHandle(cutterSimPos);
    if (m_simProcess)
    {
        m_simProcess->quit();
        m_simProcess->wait();
    }
    SimulationProgress = 100;
    
    m_simProcess.reset();
    m_pathPolyline.reset();
    m_state = IDLE;
}

void Simulator3CComponent::Reset()
{
    if (m_simProcess)
    {
        m_simProcess->quit();
        m_simProcess->wait();
        SimulationProgress = 0;
    }
    
    m_heightMap->Reset();
    m_heightMap->UploadToGPU();
    m_simProcess.reset();
    m_pathPolyline.reset();
    m_state = IDLE;
    EntityContextMenu::MakeRepaint();
}

void Simulator3CComponent::ClearPaths()
{
    if (m_simProcess)
    {
        m_simProcess->quit();
        m_simProcess->wait();
        SimulationProgress = 0;
    }
    
    m_simProcess.reset();
    m_pathPolyline.reset();
    m_state = IDLE;
    EntityContextMenu::MakeRepaint();
}

