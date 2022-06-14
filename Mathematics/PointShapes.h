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
public:
    static std::vector<QVector3D> CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints);
    static std::vector<QVector3D> CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints);
    
    static std::vector<QVector3D> CreateFillPlanePoints(std::vector<std::vector<QVector3D>> edgePoints);
};

#endif //SMOCAD_POINTSHAPES_H
