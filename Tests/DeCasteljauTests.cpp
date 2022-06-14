//
// Created by ksm on 6/14/22.
//

#include "DeCasteljauTests.h"
#include "Mathematics/Polynomials.h"

bool DeCasteljauTests::QVector3DAlmostEquals(QVector3D one, QVector3D two)
{
    return (std::abs(one.x() - two.x()) < feps) and
           (std::abs(one.y() - two.y()) < feps) and
           (std::abs(one.z() - two.z()) < feps);
}

TEST_F(DeCasteljauTests, ValueIn0)
{
    std::vector<QVector3D> knots = {
            {0, 3, 2},
            {1, 2, 3},
            {4, 3, 2},
            {1, 1, 1}
    };
    
    auto res = Polynomials::deCasteljau(0, knots);

    EXPECT_TRUE(QVector3DAlmostEquals(res, knots[0]));
}

TEST_F(DeCasteljauTests, ValueIn1)
{
    std::vector<QVector3D> knots = {
            {0, 3, 2},
            {1, 2, 3},
            {4, 3, 2},
            {1, 1, 1},
            {4, 6, 9},
            {1, 0, 1},
            {3, 2, 1}
    };
    
    auto res = Polynomials::deCasteljau(1, knots);
    
    EXPECT_TRUE(QVector3DAlmostEquals(res, knots[knots.size() - 1]));
}

TEST_F(DeCasteljauTests, ValueIn1div4)
{
    std::vector<QVector3D> knots = {
            {4, 0, 0},
            {2, 0, 0},
            {0, 0, 0},
            {1, 0, 0}
    };
    
    auto res = Polynomials::deCasteljau(0.25f, knots);
    
    EXPECT_TRUE(QVector3DAlmostEquals(res, {163.0f / 64.0f, 0, 0}));
}

TEST_F(DeCasteljauTests, StepsIn1div4)
{
    std::vector<QVector3D> knots = {
            {4, 0, 0},
            {2, 0, 0},
            {0, 0, 0},
            {1, 0, 0}
    };
    
    QVector3D** steps = new QVector3D*[4];
    for (size_t i = 0; i < 4; ++i) steps[i] = new QVector3D[4];
    
    Polynomials::deCasteljauSteps(0.25f, knots.data(), steps, 4);
    
    EXPECT_TRUE(QVector3DAlmostEquals(steps[3][0], {163.0f / 64.0f, 0, 0}));
    
    for (size_t i = 0; i < 4; ++i) delete [] steps[i];
    delete [] steps;
}


