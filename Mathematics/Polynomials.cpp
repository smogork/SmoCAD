//
// Created by ksm on 6/14/22.
//

#include "Polynomials.h"

QVector3D Polynomials::deCasteljau(float t, QVector3D *points, int size)
{
    QVector3D* data = new QVector3D[size];
    
    for (int i = 0; i < size; ++i)
        data[i] = points[i];
    
    for (int h = 1; h < size ; ++h)
        for (int i = 0; i < size - h ;  i++)
            data[i] = (1.0f - t) * data[i] + t * data[i + 1];
    
    QVector3D res = data[0];
    delete [] data;
    
    return res;
}

void Polynomials::deCasteljauSteps(float t, QVector3D *points, QVector3D **steps, int size)
{
    if (steps == nullptr)
        throw std::runtime_error("steps matrix must be passed to function");
        
    for (int i = 0; i < size; ++i)
        steps[0][i] = points[i];
    
    for (int h = 1; h < size ; ++h)
        for (int i = 0; i < size - h ;  i++)
            steps[h][i] = (1.0f - t) * steps[h - 1][i] + t * steps[h - 1][i + 1];
}
