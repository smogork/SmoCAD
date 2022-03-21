//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_VIEWPORT_H
#define SMOCAD_VIEWPORT_H


#include <QMatrix4x4>

class Viewport
{
private:
    QMatrix4x4 projectionMatrix;
    float fov;
    QSize viewportSize;

public:
    Viewport(QSize viewport, float fov);

    QMatrix4x4 UpdatePerspectiveMatrix(QSize viewport);

    QMatrix4x4 GetProjectionMatrix();
    QSize GetViewportSize();
};


#endif //SMOCAD_VIEWPORT_H
