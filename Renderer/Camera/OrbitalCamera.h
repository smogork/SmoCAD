//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_ORBITALCAMERA_H
#define SMOCAD_ORBITALCAMERA_H


#include <QVector3D>
#include <QMatrix4x4>

class OrbitalCamera
{
private:
    float r;
    float fiAngle;
    float thetaAngle;
    QVector3D rightVec;
    QVector3D topVec;

    void UpdateFrontAndRight();


public:
    QVector3D CenterPoint;

    OrbitalCamera(QVector3D centerPoint, float pivotLength);

    void RotateAroundCenter(float dFi, float dTheta);
    void SetTheta(float theta);
    void SetFi(float fi);
    void SetPivotLength(float pivotLength);
    void ChangePivotLength(float dPivotLength);
    float Pivotlength();
    void MoveUp(float moveValue);
    void MoveRight(float moveValue);
    QVector3D GetPivotVector();

    QVector3D GetPosition();
    //CenterViewPlain - płaszczyzna równoległa do neara i fara przechodząca przez centerPoint
    QVector4D GetCenterViewPlain();
    QMatrix4x4 GetViewMatrix();
};


#endif //SMOCAD_ORBITALCAMERA_H
