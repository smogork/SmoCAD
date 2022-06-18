
//
// Created by ksm on 6/18/22.
//

#ifndef SMOCAD_COMMONTESTCLASS_H
#define SMOCAD_COMMONTESTCLASS_H

#include <gtest/gtest.h>

#include <QVector3D>
#include <QVector4D>


class CommonTestClass: public testing::Test
{
protected:
    
    const float feps = 1e-5;
    
    bool QVector3DAlmostEquals(QVector3D one, QVector3D two);
    bool QVector4DAlmostEquals(QVector4D one, QVector4D two);
};

#endif //SMOCAD_COMMONTESTCLASS_H
