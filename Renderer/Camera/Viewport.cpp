//
// Created by ksm on 3/21/22.
//

#include "Viewport.h"
#include "Renderer/Renderer.h"

Viewport::Viewport(QSize viewport, float fov)
{
    this->viewportSize = viewport;
    this->fov = fov;
    this->radFov = fov * M_PIf / 180.0f;
    aspectRatio = (float)viewport.width() / (float)viewport.height();
}

QMatrix4x4 Viewport::UpdatePerspectiveMatrix(QSize viewport)
{
    this->viewportSize = viewport;
    aspectRatio = (float)viewportSize.width() / (float)viewportSize.height();

    /*projectionMatrix.setToIdentity();
    projectionMatrix.perspective(fov, (float)viewport.width() / (float)viewport.height(), 0.1f, 100.0f);
    UpdateStereoscopicMatrices();*/

    return GetProjectionMatrix();
}

QMatrix4x4 Viewport::GetProjectionMatrix()
{
    QMatrix4x4 res;
    res.perspective(fov, aspectRatio, NEAR, FAR);
    return res;
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

QMatrix4x4 Viewport::GetLeftEyeProjectionMatrix()
{
    return CreateStereoMatrix(true);
}

QMatrix4x4 Viewport::GetRightEyeProjectionMatrix()
{
    return CreateStereoMatrix(false);
}

QMatrix4x4 Viewport::CreateStereoMatrix(bool isLeft)
{
    //mirror the parameters with the right eye
    float left_right_direction = -1.0f;
    if(isLeft)
        left_right_direction = 1.0f;
    float nearZ = NEAR;
    float farZ = FAR;
    double frustumshift = (Renderer::EyeSeparation/2)*nearZ/farZ;
    float top = tan(radFov/2)*nearZ;
    float right =
            aspectRatio*top+frustumshift*left_right_direction;
//half screen
    float left =     -aspectRatio*top+frustumshift*left_right_direction;
    float bottom = -top;
    QMatrix4x4 res;
    res.frustum(left, right, bottom, top, nearZ, farZ);
    return res;
}
