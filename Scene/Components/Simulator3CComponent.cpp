//
// Created by ksm on 10/23/22.
//

#include "Simulator3CComponent.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Simulator3CSystem.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"
#include "Loaders/GCodeLoader.h"

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
    m_heightTexture = std::make_shared<QOpenGLTexture>(m_heightMap,
                                                       QOpenGLTexture::MipMapGeneration::DontGenerateMipMaps);
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
    
    m_state = IDLE;
    MoveCutterToIdleState();
}

Simulator3CComponent::~Simulator3CComponent()
{
    UnregisterComponent();
}

void Simulator3CComponent::InitializeHeightMap()
{
    m_heightMap = QImage({m_blockParams.TextureWidthX, m_blockParams.TextureWidthY}, QImage::Format_RGB32);
    QPainter p(&m_heightMap);
    p.fillRect(0, 0, m_blockParams.TextureWidthX, m_blockParams.TextureWidthY, QGradient::Preset::AfricanField);
}

void Simulator3CComponent::LoadPathFile(const QString &filepath)
{
    if (!CanLoadPaths())
        return;
    
    try
    {
        m_cutterPath = GCodeLoader::LoadCutterPath(filepath);
        m_cutterParams = m_cutterPath->Cutter;
        m_pathPolyline = std::make_unique<CutterPathPolyline>(m_cutterPath->Points, p_Transform);
        m_cutter->SetCutterParameters(m_cutterParams);
        m_cutter->p_Transform->Position = m_cutterPath->Points[0];
        m_state = PAUSED;
    }
    catch (std::runtime_error &e)
    {
        qDebug() << "Error on loading paths " << e.what();
        
        MoveCutterToIdleState();
        m_state = IDLE;
        throw std::runtime_error(QString("Error on loading paths from %0.\n%1").arg(filepath).arg(e.what()).toStdString());
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
    m_cutter->SetCutterParameters(m_cutterParams);
}

void Simulator3CComponent::ChangeCutterDiameter(Length diameter)
{
    if (!CanChangeCutterParams())
        return;
    
    m_cutterParams.Diameter = diameter;
    m_cutter->SetCutterParameters(m_cutterParams);
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

