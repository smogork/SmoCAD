//
// Created by ksm on 4/22/22.
//

#include <QtGui/QVector3D>
#include "InterpolationC2Test.h"
#include "Mathematics/Interpolation.h"
#include "gtest/internal/gtest-internal.h"

bool InterpolationC2Test::TestInterpolationResultC0(std::vector<QVector3D> &knots, std::vector<QVector3D> &res)
{
    EXPECT_EQ(res.size(), 3 * (knots.size() - 1) + 1);
    //sprawdzenie C0
    for (int i = 0; i < knots.size(); ++i)
    {
        if (!QVector3DAlmostEquals(knots[i], res[3 * i]))
            return false;
    }
    return true;
}

bool InterpolationC2Test::TestInterpolationResultC1(std::vector<QVector3D> &knots, std::vector<QVector3D> &res,
                                                    std::vector<double> &distance)
{
    for (int i = 1; i < knots.size() - 1; ++i)
    {
        QVector3D tga = qFuzzyIsNull(distance[i - 1]) ? QVector3D() : (res[3 * i] - res[3 * i - 1]) /
                                                                                      distance[i - 1];
        QVector3D tgb = qFuzzyIsNull(distance[i]) ? QVector3D() : (res[3 * i + 1] - res[3 * i]) /
                                                                                  distance[i];

        if (!QVector3DAlmostEquals(tga, tgb))
            return false;
    }
    return true;
}

bool InterpolationC2Test::TestInterpolationResultC2(std::vector<QVector3D> &knots, std::vector<QVector3D> &res,
                                                    std::vector<double> &distance)
{
    for (int i = 1; i < knots.size() - 1; ++i)
    {
        QVector3D der2a = qFuzzyIsNull(distance[i - 1]) ? QVector3D() :
                          (res[3 * i] - 2.0f * res[3 * i - 1] + res[3 * i - 2]) / (distance[i - 1] * distance[i - 1]);
        QVector3D der2b = qFuzzyIsNull(distance[i]) ? QVector3D() :
                          (res[3 * i + 2] - 2.0f * res[3 * i + 1] + res[3 * i]) / (distance[i] * distance[i]);
        if (!QVector3DAlmostEquals(der2a, der2b))
            return false;
    }
    return true;
}

bool InterpolationC2Test::QVector3DAlmostEquals(QVector3D one, QVector3D two)
{
    return (std::abs(one.x() - two.x()) < feps) and
            (std::abs(one.y() - two.y()) < feps) and
            (std::abs(one.z() - two.z()) < feps);
}

std::vector<double> InterpolationC2Test::CountDistance(std::vector<QVector3D> &knots)
{
    std::vector<double> distance(knots.size() - 1);
    for (int i = 1; i < knots.size(); ++i)
        distance[i - 1] = knots[i].distanceToPoint(knots[i - 1]);
    return std::move(distance);
}

TEST_F(InterpolationC2Test, ZeroKnots)
{
    std::vector<QVector3D> knots = {};

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_EQ(res.size(), 0);
}

TEST_F(InterpolationC2Test, SingleKnot)
{
    std::vector<QVector3D> knots = {{1, 2, 3}};
    std::vector<double> d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_EQ(res.size(), 0);
}

TEST_F(InterpolationC2Test, TwoKnotsLine)
{
    std::vector<QVector3D> knots = { {1, 2, 3}, {2, 3, 4} };
    std::vector<double> d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(QVector3DAlmostEquals({1.333333, 2.3333333, 3.333333}, res[1]));
    EXPECT_TRUE(QVector3DAlmostEquals({1.666666, 2.666666, 3.666666}, res[2]));
}

TEST_F(InterpolationC2Test, DistinctPoints)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 1, 0},
            {2, 0, 0},
            {3, -1, 0},
            {4, 0, 0},
            {5, 3, 0},
            {6, -1, 0},
            {10, 10, 0},
            {7, 0, 0},
    };
    std::vector<double> d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, DistinctPoints2)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 1, 0},
            {5, 0, 3},
            {-3, 0, 0},
            {-2, -1, 1},
            {5, 3, 0},
    };
    std::vector<double> d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, RepeatedPointC1)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 1, 0},
            {1, 1, 0},
            {2, 0, 0},
            {3, -1, 0},
            {4, 0, 0}
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_FALSE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, RepeatedPointC0)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 1, 0},
            {2, 1, 0},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_FALSE(TestInterpolationResultC1(knots, res, d));
    EXPECT_FALSE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, RepeatedFirstPointC1)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {0, 0, 0},
            {1, 0, 0},
            {1, 1, 0},
            {2, 1, 0},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_FALSE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, RepeatedFirstPointC0)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {1, 0, 0},
            {1, 1, 0},
            {2, 1, 0},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_FALSE(TestInterpolationResultC1(knots, res, d));
    EXPECT_FALSE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, CollinearDistinctPointsX)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 0, 0},
            {2, 0, 0},
            {3, 0, 0},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, CollinearDistinctPointsY)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {0, 1, 0},
            {0, 2, 0},
            {0, 3, 0},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, CollinearDistinctPointsZ)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {0, 0, 1},
            {0, 0, 2},
            {0, 0, 3},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}

TEST_F(InterpolationC2Test, CollinearDistinctPointsXYZ)
{
    std::vector<QVector3D> knots = {
            {0, 0, 0},
            {1, 1, 1},
            {2, 2, 2},
            {3, 3, 3},
            {4, 4, 4},
            {5, 5, 5},
            {6, 6, 6},
            {7, 7, 7},
            {8, 8, 8},
    };
    auto d = CountDistance(knots);

    auto res = Interpolation::C2Interpolation(knots);

    EXPECT_TRUE(TestInterpolationResultC0(knots, res));
    EXPECT_TRUE(TestInterpolationResultC1(knots, res, d));
    EXPECT_TRUE(TestInterpolationResultC2(knots, res, d));
}


