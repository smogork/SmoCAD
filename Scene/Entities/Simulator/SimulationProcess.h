//
// Created by ksm on 10/26/22.
//

#ifndef SMOCAD_SIMULATIONPROCESS_H
#define SMOCAD_SIMULATIONPROCESS_H

#include <QThread>
#include <QMutex>
#include "Scene/Utilities/QImageOperator.h"
#include "FileManagers/CutterPath.h"
#include "BlockParameters.h"

class SimulationProcess: public QThread
{
    Q_OBJECT
private:
    std::shared_ptr<QImageOperator> m_heightMap;
    std::unique_ptr<CutterPath> m_cutterPath;
    BlockParameters m_blockParams;
    int pathNumber = 0;
    const int updateDeltaTimeMs = 100;
    float lastUpdate = 0.0f;
    bool abort = false, pause = true, skip = false;
    
    QPoint CutterToTexture(QVector2D cutterP, QVector3D CutterSimPos);
    QPoint CutterCentreToTexture(QVector3D CutterSimPos);
    
    void run() override;

public slots:
    void onPlaySimulation();
    void onPauseSimulation();
    void onSkipSimulation();
    void onAbortSimulation();
    
signals:
    void SimulationResultReady(QVector3D cutterSimPos);
    void SimulationProgress(int progress);
    void SimulationError(std::string msg);
    void SimulationFinished(QVector3D cutterSimPos);

public:
    explicit SimulationProcess(std::unique_ptr<CutterPath> path, std::shared_ptr<QImageOperator> heightMap, BlockParameters blockParams);
};

#endif //SMOCAD_SIMULATIONPROCESS_H
