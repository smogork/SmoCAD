//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_POLYNOMIALS_H
#define SMOCAD_POLYNOMIALS_H

#include <QVector3D>

class Polynomials
{
public:
    static QVector3D deCasteljau(float t, QVector3D *points, int size);
    
    static QVector3D deCasteljau(float t, std::vector<QVector3D> points)
    {
        return deCasteljau(t, points.data(), (int)points.size());
    }
    
    static void deCasteljauSteps(float t, QVector3D *points, QVector3D** steps, int size);
};

#endif //SMOCAD_POLYNOMIALS_H
