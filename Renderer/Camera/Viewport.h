//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_VIEWPORT_H
#define SMOCAD_VIEWPORT_H


#include <QMatrix4x4>
#include "StereoscopicParameters.h"

class Viewport
{
private:
    QMatrix4x4 projectionMatrix, leftProjectionMatrix, rightProjectionMatrix;
    float fov;
    QSize viewportSize;

public:
    std::unique_ptr<StereoscopicParameters> StereoParams;

    Viewport(QSize viewport, float fov);

    QMatrix4x4 UpdatePerspectiveMatrix(QSize viewport);

    QMatrix4x4 GetProjectionMatrix();
    QMatrix4x4 GetLeftEyeProjectionMatrix();
    QMatrix4x4 GetRightEyeProjectionMatrix();
    QSize GetViewportSize();
    QPoint ComputeViewPoint(QVector3D NDCPoint);
};


#endif //SMOCAD_VIEWPORT_H
