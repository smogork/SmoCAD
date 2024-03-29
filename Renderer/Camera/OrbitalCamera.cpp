//
// Created by ksm on 3/12/22.
//

#include "OrbitalCamera.h"
#include "Scene/Components/Transform.h"
#include "Renderer/Options.h"
#include <cfloat>

OrbitalCamera::OrbitalCamera(QVector3D centerPoint, float pivotLength)
{
    Reset();
    CenterPoint = centerPoint;
    SetPivotLength(pivotLength);
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
    CenterPoint += moveValue * topVec * r;
}

void OrbitalCamera::MoveRight(float moveValue)
{
    CenterPoint += moveValue * rightVec * r;
}

void OrbitalCamera::UpdateFrontAndRight()
{
    QVector3D frontVec = -GetPivotVector();
    rightVec = QVector3D::crossProduct(frontVec, Transform::GetYAxis()).normalized();
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
    return QVector3D(
            cosf(thetaAngle) * cosf(fiAngle),
            sinf(thetaAngle),
            sinf(fiAngle) * cosf(thetaAngle));
}

float OrbitalCamera::Pivotlength()
{
    return r;
}

QMatrix4x4 OrbitalCamera::GetLeftEyeViewMatrix()
{
    QMatrix4x4 res;
    float scale = Options::StereoParams->EyeSeparation / 2;
    res.lookAt(
            GetPosition() - scale * rightVec ,
            CenterPoint - scale * rightVec,
            Transform::GetYAxis()
    );
    return res;
}

QMatrix4x4 OrbitalCamera::GetRightEyeViewMatrix()
{
    QMatrix4x4 res;
    float scale = Options::StereoParams->EyeSeparation / 2;
    res.lookAt(
            GetPosition() + scale * rightVec,
            CenterPoint + scale * rightVec,
            Transform::GetYAxis()
    );
    return res;
}

QMatrix4x4 OrbitalCamera::CreateStereoMatrix(bool isLeft)
{
    float left_right_direction = -1.0f;
    if (isLeft)
        left_right_direction = 1.0f;

    QMatrix4x4 res;
    res.lookAt(
            GetPosition() - rightVec *  Options::StereoParams->EyeSeparation / 2,
            CenterPoint - rightVec *  Options::StereoParams->EyeSeparation / 2,
            Transform::GetYAxis()
        );
    return res;
}

void OrbitalCamera::Reset()
{
    CenterPoint = QVector3D();
    fiAngle = 0;
    thetaAngle = M_PI / 4;
    SetPivotLength(5);
    UpdateFrontAndRight();
}

void OrbitalCamera::MoveForward(float moveValue)
{
    QVector3D front = (-GetPivotVector());
    front.setY(0);
    CenterPoint += moveValue * front.normalized() * r;
}


