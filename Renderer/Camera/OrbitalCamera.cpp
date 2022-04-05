//
// Created by ksm on 3/12/22.
//

#include "OrbitalCamera.h"
#include "Objects/TransformableObject.h"
#include "Scene/Components/Transform.h"
#include <cfloat>

OrbitalCamera::OrbitalCamera(QVector3D centerPoint, float pivotLength)
{
    CenterPoint = centerPoint;
    SetPivotLength(pivotLength);
    fiAngle = 0;
    thetaAngle = 0;
    UpdateFrontAndRight();
}

QMatrix4x4 OrbitalCamera::GetViewMatrix()
{
    QMatrix4x4 res;
    res.lookAt(GetPosition(), CenterPoint, Transform::GetYAxis());
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
    UpdateFrontAndRight();
}

void OrbitalCamera::SetFi(float fi)
{
    fiAngle = fi;
    UpdateFrontAndRight();
}

void OrbitalCamera::SetPivotLength(float pivotLength)
{
    r = std::clamp(pivotLength, 1.0f, FLT_MAX);
}

QVector3D OrbitalCamera::GetPosition()
{
    return CenterPoint + r * GetPivotVector();
}

void OrbitalCamera::ChangePivotLength(float dPivotLength)
{
    SetPivotLength(r + dPivotLength);
}

void OrbitalCamera::MoveUp(float moveValue)
{
    CenterPoint += moveValue * topVec;
}

void OrbitalCamera::MoveRight(float moveValue)
{
    CenterPoint += moveValue * rightVec;
}

void OrbitalCamera::UpdateFrontAndRight()
{
    QVector3D frontVec = - GetPivotVector();
    rightVec = QVector3D::crossProduct(frontVec, TransformableObject::GetYAxis()).normalized();
    topVec = QVector3D::crossProduct(rightVec, frontVec);
}

QVector4D OrbitalCamera::GetCenterViewPlain()
{
    QVector3D back = GetPivotVector();

    return {
            back.x(),
            back.y(),
            back.z(),
            QVector3D::dotProduct(back, -CenterPoint)
    };
}

QVector3D OrbitalCamera::GetPivotVector()
{
    return QVector3D (
            cosf(thetaAngle) * cosf(fiAngle),
            sinf(thetaAngle),
            sinf(fiAngle) * cosf(thetaAngle));
}


