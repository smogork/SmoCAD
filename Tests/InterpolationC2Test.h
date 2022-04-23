//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_INTERPOLATIONC2TEST_H
#define SMOCAD_INTERPOLATIONC2TEST_H

#include <gtest/gtest.h>

class InterpolationC2Test: public testing::Test
{
    const float feps = 1e-6;

public:
    InterpolationC2Test( ) {
        // initialization;
        // can also be done in SetUp()
    }

    void SetUp( ) {
        // initialization or some code to run before each test
    }

    void TearDown( ) {
        // code to run after each test;
        // can be used instead of a destructor,
        // but exceptions can be handled in this function only
    }

    ~InterpolationC2Test( ) override {
        //resources cleanup, no exceptions allowed
    }

    // shared user data
    bool TestInterpolationResultC0(std::vector<QVector3D>& knots, std::vector<QVector3D>& res);
    bool TestInterpolationResultC1(std::vector<QVector3D>& knots, std::vector<QVector3D>& res, std::vector<double>& distance);
    bool TestInterpolationResultC2(std::vector<QVector3D>& knots, std::vector<QVector3D>& res, std::vector<double>& distance);
    bool QVector3DAlmostEquals(QVector3D one, QVector3D two);
    std::vector<double> CountDistance(std::vector<QVector3D>& knots);
};

#endif //SMOCAD_INTERPOLATIONC2TEST_H
