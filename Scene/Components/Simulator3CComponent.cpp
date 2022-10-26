//
// Created by ksm on 10/23/22.
//

#include "Simulator3CComponent.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Simulator3CSystem.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"
#include "Loaders/GCodeLoader.h"

#include <cmath>

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
    InitializeHeightMap();
    
    m_heightTexture = std::make_shared<QOpenGLTexture>(m_heightMap.GetBitmap(),
                                                       QOpenGLTexture::MipMapGeneration::DontGenerateMipMaps);
    m_heightTexture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::NearestMipMapLinear);
    m_heightTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
    
    m_blockLower = std::make_unique<BlockLowerWall>(QVector3D(), p_Transform, m_blockParams.WidthX.GetSceneUnits(),
                                                    m_blockParams.WidthY.GetSceneUnits());
    m_blockSide = std::make_unique<BlockSideWall>(QVector3D(), p_Transform, m_heightTexture,
                                                  m_blockParams.WidthX.GetSceneUnits(),
                                                  m_blockParams.WidthY.GetSceneUnits(),
                                                  m_blockParams.Height.GetSceneUnits(), m_blockParams.VertexWidthX,
                                                  m_blockParams.VertexWidthY);
    m_blockUpper = std::make_unique<BlockUpperWall>(QVector3D(0, m_blockParams.Height.GetSceneUnits(), 0), p_Transform,
                                                    m_heightTexture,
                                                    m_blockParams.WidthX.GetSceneUnits(),
                                                    m_blockParams.WidthY.GetSceneUnits(),
                                                    m_blockParams.Height.GetSceneUnits(),
                                                    m_blockParams.VertexWidthX, m_blockParams.VertexWidthY);
    m_cutter = std::make_unique<CutterObject>(QVector3D(), m_cutterParams, p_Transform);
    
    ChangeCutterType(m_cutterParams.Type);
    m_state = IDLE;
    MoveCutterToIdleState();
}

Simulator3CComponent::~Simulator3CComponent()
{
    UnregisterComponent();
}

void Simulator3CComponent::InitializeHeightMap()
{
    m_heightMap.ChangeSize(m_blockParams.TextureWidthX, m_blockParams.TextureWidthY);
    //m_heightMap.GetRedColorFunction = ASSIGN_COLOR_FUNCTION(&Simulator3CComponent::CutterColorFunction);
}

void Simulator3CComponent::LoadPathFile(const QString &filepath)
{
    if (!CanLoadPaths())
        return;
    
    try
    {
        m_cutterPath = GCodeLoader::LoadCutterPath(filepath);
        ChangeCutterParameters(m_cutterPath->Cutter);
        m_pathPolyline = std::make_unique<CutterPathPolyline>(m_cutterPath->Points, p_Transform);
        m_cutter->p_Transform->Position = m_cutterPath->Points[0];
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

QPoint Simulator3CComponent::CutterToTexture(QVector2D cutterP, QVector3D CutterSimPos)
{
    return QPoint(
            ((cutterP.x() + CutterSimPos.x()) / m_blockParams.WidthX.GetSceneUnits() + 0.5f) *
            m_blockParams.TextureWidthX,
            ((cutterP.y() + CutterSimPos.z()) / m_blockParams.WidthY.GetSceneUnits() + 0.5f) *
            m_blockParams.TextureWidthY
    );
}

QPoint Simulator3CComponent::CutterCentreToTexture(QVector3D CutterSimPos)
{
    return CutterToTexture({0, 0}, CutterSimPos);
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
    
    for (int i = 0; i < m_cutterPath->Points.size() - 1; ++i)
    {
        QVector3D startPoint = m_cutterPath->Points[i];
        QVector3D finishPoint = m_cutterPath->Points[i + 1];
        QPoint startPointTex = CutterCentreToTexture(startPoint);
        QPoint endPointTex = CutterCentreToTexture(finishPoint);
        
        m_heightMap.CutterMove(startPointTex, endPointTex, startPoint.y(), finishPoint.y(), m_blockParams.Height.GetSceneUnits());
        
        qDebug() << "Path nr" << i;
    }
    
    m_heightTexture->setData(m_heightMap.GetBitmap());
    m_cutterPath.reset();
    m_pathPolyline.reset();
    m_state = IDLE;
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
            m_heightMap.PrepareCylindricalStamp(GetCutterTextureRadius().x(), GetCutterTextureRadius().y(),
                                                m_cutterParams.Diameter.GetSceneUnits() / 2.0f);
            break;
        case Spherical:
            m_heightMap.PrepareSphericalStamp(GetCutterTextureRadius().x(), GetCutterTextureRadius().y(),
                                              m_cutterParams.Diameter.GetSceneUnits() / 2.0f);
            break;
    }
}

