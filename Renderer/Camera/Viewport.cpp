//
// Created by ksm on 3/21/22.
//

#include "Viewport.h"

Viewport::Viewport(QSize viewport, float fov)
{
    this->viewportSize = viewport;
    this->fov = fov;
}

QMatrix4x4 Viewport::UpdatePerspectiveMatrix(QSize viewport)
{
    this->viewportSize = viewport;
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(fov, (float)viewport.width() / (float)viewport.height(), 0.1f, 100.0f);
    return GetProjectionMatrix();
}

QMatrix4x4 Viewport::GetProjectionMatrix()
{
    return projectionMatrix;
}

QSize Viewport::GetViewportSize()
{
    return viewportSize;
}

QPoint Viewport::ComputeViewPoint(QVector3D NDCPoint)
{
    return QPoint(
            2.0f * ((NDCPoint.x() - 1.0f) / viewportSize.width()) - 1.0f,
            2.0f * ((viewportSize.height() - NDCPoint.y() + 1.0f) / viewportSize.height()) - 1.0f
            );
}
