//
// Created by ksm on 3/12/22.
//

#include "OrbitalCamera.h"
#include "Renderer/SceneRenderer.h"

OrbitalCamera::OrbitalCamera(QVector3D centerPoint, float pivotLength)
{
    CenterPoint = centerPoint;
    SetPivotLength(pivotLength);
    fiAngle = 0;
    thetaAngle = 0;
}

QMatrix4x4 OrbitalCamera::GetViewMatrix()
{
    QMatrix4x4 res;
    res.lookAt(GetPosition(), CenterPoint, SceneRenderer::GetYAxis());
    return res;
}

void OrbitalCamera::RotateAroundCenter(float dFi, float dTheta)
{
    SetFi(fiAngle + dFi);
    SetTheta(thetaAngle + dTheta);
}

void OrbitalCamera::SetTheta(float theta)
{
    if (theta < -M_PI_2 || theta > M_PI_2)
        return;
    thetaAngle = theta;
}

void OrbitalCamera::SetFi(float fi)
{
    fiAngle = fi;
}

void OrbitalCamera::SetPivotLength(float pivotLength)
{
    r = std::clamp(pivotLength, 0.0f, MAXFLOAT);
}

QVector3D OrbitalCamera::GetPosition()
{
    QVector3D pivot(
            r * cosf(thetaAngle) * cosf(fiAngle),
            r * sinf(thetaAngle),
            r * sinf(fiAngle) * cosf(thetaAngle));
    return CenterPoint + pivot;
}


