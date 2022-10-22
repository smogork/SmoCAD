//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_POINTSHAPES_H
#define SMOCAD_POINTSHAPES_H

#include <QVector3D>

enum Plane
{
    XY,
    XZ,
    YZ
};

class PointShapes
{
private:
    static void ApplyPlaneTransform(std::vector<QVector3D> &points, QVector3D startPos, Plane plane);
    
public:
    static std::vector<QVector3D> CreateSingleRect(QVector3D centerPos, float width, float height, Plane plane = XZ);
    static std::vector<QVector3D> CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints, Plane plane = XZ);
    static std::vector<QVector3D> CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints);
    
    static std::vector<QVector3D> CreateFillPlanePoints(std::vector<QVector3D> edgePoints);
};

#endif //SMOCAD_POINTSHAPES_H
