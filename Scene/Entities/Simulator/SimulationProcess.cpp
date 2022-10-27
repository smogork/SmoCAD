//
// Created by ksm on 10/26/22.
//

#include <QElapsedTimer>
#include <QDebug>
#include "SimulationProcess.h"

SimulationProcess::SimulationProcess(std::unique_ptr<CutterPath> path, std::shared_ptr<QImageOperator> heightMap, BlockParameters blockParams) :
        m_cutterPath(std::move(path)), m_blockParams(blockParams), m_heightMap(heightMap)
{

}

void SimulationProcess::onPlaySimulation()
{
    pause = false;
}

void SimulationProcess::onPauseSimulation()
{
    pause = true;
}

void SimulationProcess::onSkipSimulation()
{
    skip = true;
    pause = false;
}


void SimulationProcess::onAbortSimulation()
{
    abort = true;
}

QPoint SimulationProcess::CutterToTexture(QVector2D cutterP, QVector3D CutterSimPos)
{
    return QPoint(
            ((cutterP.x() + CutterSimPos.x()) / m_blockParams.WidthX.GetSceneUnits() + 0.5f) *
            m_blockParams.TextureWidthX,
            ((cutterP.y() + CutterSimPos.z()) / m_blockParams.WidthY.GetSceneUnits() + 0.5f) *
            m_blockParams.TextureWidthY
    );
}

QPoint SimulationProcess::CutterCentreToTexture(QVector3D CutterSimPos)
{
    return CutterToTexture({0, 0}, CutterSimPos);
}

void SimulationProcess::run()
{
    QElapsedTimer timer;
    timer.start();
    
    for (; pathNumber < m_cutterPath->Points.size() - 1; ++pathNumber )
    {
        while (pause && !abort && !skip)
            msleep(200);
    
        if (abort)
            return;
        
        QVector3D startPoint = m_cutterPath->Points[pathNumber];
        QVector3D finishPoint = m_cutterPath->Points[pathNumber + 1];
        QPoint startPointTex = CutterCentreToTexture(startPoint);
        QPoint endPointTex = CutterCentreToTexture(finishPoint);
        
        try
        {
            m_heightMap->CutterMove(startPointTex, endPointTex, startPoint.y(), finishPoint.y(),
                                    m_blockParams.Height.GetSceneUnits());
        }
        catch (MillingException &e)
        {
            abort = true;
            emit SimulationError(e.what());
            emit SimulationFinished(m_cutterPath->Points[pathNumber]);
        }
        
        if (abort)
            return;
        
        if (timer.elapsed() > updateDeltaTimeMs)
        {
            if (!skip)
                emit SimulationResultReady(finishPoint);
            
            emit SimulationProgress((float)pathNumber / m_cutterPath->Points.size() * 100);
            timer.restart();
        }
    }
    
    emit SimulationFinished(m_cutterPath->Points[pathNumber]);
}

