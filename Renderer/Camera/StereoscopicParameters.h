//
// Created by ksm on 5/9/22.
//

#ifndef SMOCAD_STEREOSCOPICPARAMETERS_H
#define SMOCAD_STEREOSCOPICPARAMETERS_H

#include <QVector3D>

class StereoscopicParameters
{
private:
    static const QVector3D m_leftRightDir;

public:
    double EyeDistance = 0.1;
};

#endif //SMOCAD_STEREOSCOPICPARAMETERS_H
