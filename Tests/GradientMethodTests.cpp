//
// Created by ksm on 6/18/22.
//

#include "GradientMethodTests.h"
#include "Mathematics/Optimization.h"

float GradientMethodTests::Paraboloid(QVector4D x, float a, float b)
{
    return x.x() * x.x() * a + x.y() * x.y() * b;
}

QVector4D GradientMethodTests::ParaboloidGrad(QVector4D x, float a, float b)
{
    return {
            2 * x.x() * a,
            2 * x.y() * b,
            0, 0
    };
}

TEST_F(GradientMethodTests, ParaboloidAtMinimum)
{
    float a = 1, b = 2;
    float alpha = 0.05;
    float eps = feps * 1e-2;//aby uzyskac feps nalezy obnizyc dokladnosc o 2 rzedy wielkosci
    
    auto res = Optimization::SimpleGradientMethod(
            QVector4D(),
            std::bind(&GradientMethodTests::Paraboloid, std::placeholders::_1, a, b),
            std::bind(&GradientMethodTests::ParaboloidGrad, std::placeholders::_1, a, b), alpha, eps);
    
    EXPECT_TRUE(QVector4DAlmostEquals(res, {0, 0, 0, 0}));
}

TEST_F(GradientMethodTests, ParaboloidSimple)
{
    float a = 4, b = 2;
    float alpha = 0.05;
    float eps = feps * 1e-2;//aby uzyskac feps nalezy obnizyc dokladnosc o 2 rzedy wielkosci
    
    auto res = Optimization::SimpleGradientMethod(
            {3, 4, 0, 0},
            std::bind(&GradientMethodTests::Paraboloid, std::placeholders::_1, a, b),
            std::bind(&GradientMethodTests::ParaboloidGrad, std::placeholders::_1, a, b), alpha, eps);
    
    EXPECT_TRUE(QVector4DAlmostEquals(res, {0, 0, 0, 0}));
}


