//
// Created by ksm on 6/18/22.
//

#include "CommonTestClass.h"

bool CommonTestClass::QVector3DAlmostEquals(QVector3D one, QVector3D two)
{
    return (std::abs(one.x() - two.x()) < feps) and
           (std::abs(one.y() - two.y()) < feps) and
           (std::abs(one.z() - two.z()) < feps);
}

bool CommonTestClass::QVector4DAlmostEquals(QVector4D one, QVector4D two)
{
    return (std::abs(one.x() - two.x()) < feps) and
           (std::abs(one.y() - two.y()) < feps) and
           (std::abs(one.z() - two.z()) < feps) and
           (std::abs(one.w() - two.w()) < feps);
}
