//
// Created by ksm on 10/23/22.
//

#ifndef SMOCAD_SIMULATOR3CCOMPONENT_H
#define SMOCAD_SIMULATOR3CCOMPONENT_H

#include "IComponent.h"
#include "Scene/Entities/Simulator/CutterObject.h"
#include "Scene/Entities/Simulator/CutterPath.h"
#include "Scene/Entities/Simulator/BlockUpperWall.h"
#include "Scene/Entities/Simulator/BlockLowerWall.h"
#include "Scene/Entities/Simulator/BlockSideWall.h"
#include "Scene/Entities/Simulator/BlockParameters.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"
#include "Scene/Utilities/QImageOperator.h"



enum SimulatorState
{
    IDLE,
    MILLING,
    PAUSED,
};

class Simulator3CComponent : public IComponent
{
Q_OBJECT
private:
    std::unique_ptr<CutterObject> m_cutter;
    std::unique_ptr<CutterPath> m_cutterPath;
    std::unique_ptr<CutterPathPolyline> m_pathPolyline;
    std::unique_ptr<BlockUpperWall> m_blockUpper;
    std::unique_ptr<BlockLowerWall> m_blockLower;
    std::unique_ptr<BlockSideWall> m_blockSide;
    BlockParameters m_blockParams;
    CutterParameters m_cutterParams;
    QProperty<SimulatorState> m_state;
    
    //QImage m_heightMap;
    QImageOperator m_heightMap;
    std::shared_ptr<QOpenGLTexture> m_heightTexture;
    
    void InitializeHeightMap();
    void MoveCutterToIdleState();
    float CutterColorFunction(int x, int y, QVector3D startCutterPos, QVector3D endCutterPos);
    QPoint CutterToTexture(QVector2D cutterP, QVector3D CutterSimPos);
    QPoint CutterCentreToTexture(QVector3D CutterSimPos);
    float CutterHeightToTextureColor(float cutterHeight, float startHeight, float finishHeight);
    QVector2D TextureToCutter(int texX, int texY, QVector3D CutterSimPos);
    QVector2D TextureToSim(int texX, int texY);
    float DistToSegment(QVector2D p, QVector2D A, QVector2D B);
    void SkipPathToEnd();
    QPoint GetCutterTextureRadius();

signals:
    void SimulatorStateChanged(SimulatorState state);

public:
    static std::shared_ptr<Simulator3CComponent>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans);
    void UnregisterComponent();
    
    std::shared_ptr<Transform> p_Transform;
    
    Simulator3CComponent(unsigned int oid, std::shared_ptr<Transform> simulatorTransform);
    explicit Simulator3CComponent(unsigned int oid);
    ~Simulator3CComponent() override;
    
    SimulatorState GetState() { return m_state; }
    bool CanLoadPaths();
    void LoadPathFile(const QString& filepath);
    
    bool CanChangeCutterParams();
    void ChangeCutterType(CutterType type);
    void ChangeCutterDiameter(Length diameter);
    CutterParameters GetCutterParameters();
    
    BlockParameters GetBlockParameters();
};

#endif //SMOCAD_SIMULATOR3CCOMPONENT_H
