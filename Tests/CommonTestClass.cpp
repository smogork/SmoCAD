//
// Created by ksm on 6/18/22.
//

#include "CommonTestClass.h"
#include "Scene/Utilities/Utilites.h"

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

TEST_F(CommonTestClass, fmodfWrappingNegativeTest)
{
    float min = 1, max = 3.5;
    float v = 0, exp = 2.5;

    auto res = wrap(v, min, max);

    EXPECT_FLOAT_EQ(exp, res);
}

TEST_F(CommonTestClass, fmodfWrappingPositiveTest)
{
    float min = 1, max = 3.5;
    float v = 4, exp = 1.5;

    auto res = wrap(v, min, max);

    EXPECT_FLOAT_EQ(exp, res);
}

TEST_F(CommonTestClass, fmodfWrappingBaseTest)
{
    float min = 1, max = 3.5;
    float v = 2.5, exp = 2.5;

    auto res = wrap(v, min, max);

    EXPECT_FLOAT_EQ(exp, res);
}