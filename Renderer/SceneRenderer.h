//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_SCENERENDERER_H
#define SMOCAD_SCENERENDERER_H


#include <QVector3D>

class SceneRenderer
{
public:
    static QVector3D GetXAxis() { return {1.0, 0.0, 0.0}; }
    static QVector3D GetYAxis() { return {0.0, 1.0, 0.0}; }
    static QVector3D GetZAxis() { return {0.0, 0.0, 1.0}; }
};


#endif //SMOCAD_SCENERENDERER_H
