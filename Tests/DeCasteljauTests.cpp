//
// Created by ksm on 6/14/22.
//

#include "DeCasteljauTests.h"
#include "Mathematics/Polynomials.h"

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

TEST_F(DeCasteljauTests, DoubleFunction)
{
    std::vector<QVector3D> knots = {
            {0, 3, 2},
            {1, 2, 3},
            {4, 3, 2},
            {1, 1, 1}
    };
    
    auto doubles = Polynomials::deCasteljauDouble(0.5f, knots);
    
    EXPECT_TRUE(QVector3DAlmostEquals(knots[0], Polynomials::deCasteljau(0, doubles.first)));
    EXPECT_TRUE(QVector3DAlmostEquals(knots[3], Polynomials::deCasteljau(1, doubles.second)));
    EXPECT_TRUE(QVector3DAlmostEquals(Polynomials::deCasteljau(0.25f, knots), Polynomials::deCasteljau(0.5f, doubles.first)));
    EXPECT_TRUE(QVector3DAlmostEquals(Polynomials::deCasteljau(0.75f, knots), Polynomials::deCasteljau(0.5f, doubles.second)));
}

TEST_F(DeCasteljauTests, FirstDerivative)
{
    std::vector<float> knots = {0, 1, 2, 3};
    
    std::vector<float> der = Polynomials::deCasteljauDerK(1, knots);
    float der_value = Polynomials::deCasteljau(0.5f, der);
    
    EXPECT_EQ(der.size(), knots.size() - 1);
    EXPECT_FLOAT_EQ(der_value, 3.0f);
}

TEST_F(DeCasteljauTests, SecondDerivative)
{
    std::vector<float> knots = {0, 1, 2, 3};
    
    std::vector<float> der = Polynomials::deCasteljauDerK(2, knots);
    float der_value = Polynomials::deCasteljau(0.5f, der);
    
    EXPECT_EQ(der.size(), knots.size() - 2);
    EXPECT_FLOAT_EQ(der_value, 0.0f);
}


