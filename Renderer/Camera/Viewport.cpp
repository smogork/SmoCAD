//
// Created by ksm on 3/21/22.
//

#include "Viewport.h"
#include "Renderer/Options.h"

Viewport::Viewport(QSize viewport, float fov)
{
    this->viewportSize = viewport;
    this->fov = fov;
    this->radFov = fov * M_PI / 180.0f;
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
    res.perspective(fov, aspectRatio, NEAR_DIST, FAR_DIST);
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
    float top, bottom, left, right;

    top     = NEAR_DIST * tan(radFov/2);
    bottom  = -top;

    float a = aspectRatio * tan(radFov/2) * Options::StereoParams->FocusDistance;

    float b = a - Options::StereoParams->EyeSeparation/2;
    float c = a + Options::StereoParams->EyeSeparation/2;

    left    = -b * NEAR_DIST/Options::StereoParams->FocusDistance;
    right   =  c * NEAR_DIST/Options::StereoParams->FocusDistance;

    QMatrix4x4 res;
    res.frustum(left, right, bottom, top,
              NEAR_DIST, FAR_DIST);
    return res;
}

QMatrix4x4 Viewport::GetRightEyeProjectionMatrix()
{
    float top, bottom, left, right;

    top     = NEAR_DIST * tan(radFov/2);
    bottom  = -top;

    float a = aspectRatio * tan(radFov/2) * Options::StereoParams->FocusDistance;

    float b = a - Options::StereoParams->EyeSeparation/2;
    float c = a + Options::StereoParams->EyeSeparation/2;

    left    =  -c * NEAR_DIST/Options::StereoParams->FocusDistance;
    right   =   b * NEAR_DIST/Options::StereoParams->FocusDistance;

    QMatrix4x4 res;
    res.frustum(left, right, bottom, top,
                NEAR_DIST, FAR_DIST);
    return res;
}

QMatrix4x4 Viewport::CreateStereoMatrix(bool isLeft)
{
    //mirror the parameters with the right eye
    float left_right_direction = -1.0f;
    if(isLeft)
        left_right_direction = 1.0f;
    float nearZ = NEAR_DIST;
    float farZ = FAR_DIST;
    double frustumshift = (Options::StereoParams->EyeSeparation /2)*nearZ/farZ;
    float top = tan(radFov/2)*nearZ;
    float right =
            aspectRatio*top+frustumshift*left_right_direction;
//half screen
    float left =     -aspectRatio*top+frustumshift*left_right_direction;
    float bottom = -top;
    QMatrix4x4 res;
    res.frustum(left, right, bottom, top, nearZ, farZ);

    auto proj = GetProjectionMatrix();
    Q_ASSERT(res != proj);
    return res;
}
