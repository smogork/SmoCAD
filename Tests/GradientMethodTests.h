//
// Created by ksm on 6/18/22.
//

#ifndef SMOCAD_GRADIENTMETHODTESTS_H
#define SMOCAD_GRADIENTMETHODTESTS_H

#include "CommonTestClass.h"

class GradientMethodTests: public CommonTestClass
{
protected:
    static float Paraboloid(QVector4D x, float a, float b);
    static QVector4D ParaboloidGrad(QVector4D x, float a, float b);
};

#endif //SMOCAD_GRADIENTMETHODTESTS_H
