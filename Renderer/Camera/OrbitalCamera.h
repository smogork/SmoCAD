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

public:
    QVector3D CenterPoint;

    OrbitalCamera(QVector3D centerPoint, float pivotLength);

    QMatrix4x4 GetViewMatrix();

    void RotateAroundCenter(float dFi, float dTheta);

    void SetTheta(float theta);

    void SetFi(float fi);

    void SetPivotLength(float pivotLength);

    QVector3D GetPosition();
};


#endif //SMOCAD_ORBITALCAMERA_H
