//
// Created by ksm on 10/23/22.
//

#ifndef SMOCAD_SIMULATOR3CCOMPONENT_H
#define SMOCAD_SIMULATOR3CCOMPONENT_H

#include "IComponent.h"
#include "Scene/Entities/Simulator/CutterObject.h"
#include "FileManagers/CutterPath.h"
#include "Scene/Entities/Simulator/BlockUpperWall.h"
#include "Scene/Entities/Simulator/BlockLowerWall.h"
#include "Scene/Entities/Simulator/BlockSideWall.h"
#include "Scene/Entities/Simulator/BlockParameters.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"
#include "Scene/Utilities/QImageOperator.h"
#include "Scene/Entities/Simulator/SimulationProcess.h"

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

    std::unique_ptr<CutterPathPolyline> m_pathPolyline;
    std::unique_ptr<BlockUpperWall> m_blockUpper;
    std::unique_ptr<BlockLowerWall> m_blockLower;
    std::unique_ptr<BlockSideWall> m_blockSide;
    BlockParameters m_blockParams;
    CutterParameters m_cutterParams;

    std::unique_ptr<SimulationProcess> m_simProcess;
    
    //QImage m_heightMap;
    std::shared_ptr<QImageOperator> m_heightMap;
    //std::shared_ptr<QOpenGLTexture> m_heightTexture;
    std::shared_ptr<QOpenGLTexture> m_woodTexture;
    
    void InitializeHeightMap();
    void MoveCutterToIdleState();
    float CutterColorFunction(int x, int y, QVector3D startCutterPos, QVector3D endCutterPos);

    float CutterHeightToTextureColor(float cutterHeight, float startHeight, float finishHeight);
    QVector2D TextureToCutter(int texX, int texY, QVector3D CutterSimPos);
    QVector2D TextureToSim(int texX, int texY);
    float DistToSegment(QVector2D p, QVector2D A, QVector2D B);

    QPoint GetCutterTextureRadius();
    void ChangeCutterParameters(CutterParameters params);
    void ResizeBlock();
    
private slots:
    void onSimulationResultsHandle(QVector3D cutterSimPos);
    void onSimulationProgress(int progress);
    void onSimulationError(std::string msg);
    void onSimulationFinished(QVector3D cutterSimPos);

signals:
    void SimulatorStateChanged(SimulatorState state);
    void PlaySimulation();
    void PauseSimulation();
    void SkipSimulation();
    void AbortSimulation();

public:
    static std::shared_ptr<Simulator3CComponent>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans);
    void UnregisterComponent();
    
    std::shared_ptr<Transform> p_Transform;
    QProperty<int> SimulationProgress;
    QProperty<SimulatorState> m_state;
    
    Simulator3CComponent(unsigned int oid, std::shared_ptr<Transform> simulatorTransform);
    explicit Simulator3CComponent(unsigned int oid);
    ~Simulator3CComponent() override;
    
    SimulatorState GetState() { return m_state; }
    bool CanLoadPaths();
    void LoadPathFile(const QString& filepath);
    void SkipPathToEnd();
    void HidePathsOnScene(bool hide);
    bool GetPathsHide();
    
    bool CanChangeCutterParams();
    void ChangeCutterType(CutterType type);
    void ChangeCutterDiameter(Length diameter);
    void ChangeToolSubmersions(Length toolSub, Length globalSub);
    CutterParameters GetCutterParameters();
    std::tuple<Length, Length> GetToolSubmersions();
    
    BlockParameters GetBlockParameters();
    void ChangeBlockSize(Length X, Length Y, Length Height);
    void ChangeBlockVertices(int X, int Y);
    void ChangeTextureSize(int size);
    
    void PlayPauseSimulation();
    void Reset();
    void ClearPaths();
};

#endif //SMOCAD_SIMULATOR3CCOMPONENT_H
