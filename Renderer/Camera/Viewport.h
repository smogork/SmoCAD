//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_VIEWPORT_H
#define SMOCAD_VIEWPORT_H


#include <QMatrix4x4>
#include <QProperty>

#define NEAR_DIST 0.1f
#define FAR_DIST 100.0f


class Viewport
{
private:
    QMatrix4x4 projectionMatrix, leftProjectionMatrix, rightProjectionMatrix;
    float fov, radFov;
    QSize viewportSize;
    float aspectRatio;
    QMatrix4x4 CreateStereoMatrix(bool isLeft);

public:

    Viewport(QSize viewport, float fov);

    QMatrix4x4 UpdatePerspectiveMatrix(QSize viewport);

    QMatrix4x4 GetProjectionMatrix();
    QMatrix4x4 GetLeftEyeProjectionMatrix();
    QMatrix4x4 GetRightEyeProjectionMatrix();
    QSize GetViewportSize();
    QPoint ComputeViewPoint(QVector3D NDCPoint);
};


#endif //SMOCAD_VIEWPORT_H
