//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_POINTSHAPES_H
#define SMOCAD_POINTSHAPES_H

#include <QVector3D>

class PointShapes
{
public:
    static std::vector<QVector3D> CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints);
    static std::vector<QVector3D> CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints);
};

#endif //SMOCAD_POINTSHAPES_H
