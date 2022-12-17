//
// Created by ksm on 12/9/22.
//

#include "GeomteryRelationsTests.h"
#include "Mathematics/GeometryRelation.h"

TEST_F(GeomteryRelationsTests, SegmentRect_OuterBoth_Miss)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {5, 6};
    QPoint B = {7, 4};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, false);
}

TEST_F(GeomteryRelationsTests, SegmentRect_OuterBoth_Hit)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {0, 6};
    QPoint B = {7, 0};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentRect_InOutHorizontal_Hit)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {3, 3};
    QPoint B = {7, 3};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentRect_InOutVertical_Hit)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {3, 3};
    QPoint B = {3, 5};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentRect_OuterBothVertices_Hit)
{
    QRect rect(QPoint(1, 1), QPoint(3, 3));
    QPoint A = {0, 4};
    QPoint B = {4, 0};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentRect_OuterBothVertex_Hit)
{
    QRect rect(QPoint(1, 1), QPoint(3, 3));
    QPoint A = {2, 4};
    QPoint B = {4, 2};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentRect_OuterBothToFar_Miss)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {1, 7};
    QPoint B = {2, 5};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, false);
}

TEST_F(GeomteryRelationsTests, SegmentRect_InnerBoth_Hit)
{
    QRect rect(QPoint(1, 2), QPoint(5, 4));
    QPoint A = {2, 2};
    QPoint B = {2, 4};

    auto res = GeometryRelation::DoesSegmentCrossRect(A, B, rect);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentSegment_Hit)
{
    QPoint A1 = {2, 3};
    QPoint B1 = {2, 5};
    QPoint A2 = {2, 3};
    QPoint B2 = {5, 3};

    auto res1 = GeometryRelation::DoesSegmentsCross(A1, B1, A2, B2);
    auto res2 = GeometryRelation::DoesSegmentsCross(A2, B2, A1, B1);

    EXPECT_EQ(res1, true);
    EXPECT_EQ(res2, true);
}

TEST_F(GeomteryRelationsTests, SegmentSegment_Miss)
{
    QPoint A1 = {3, 2};
    QPoint B1 = {3, 5};
    QPoint A2 = {1, 2};
    QPoint B2 = {1, 5};

    auto res1 = GeometryRelation::DoesSegmentsCross(A1, B1, A2, B2);
    auto res2 = GeometryRelation::DoesSegmentsCross(A2, B2, A1, B1);

    EXPECT_EQ(res1, false);
    EXPECT_EQ(res2, false);
}

TEST_F(GeomteryRelationsTests, SegmentSegment_Collinear_Hit)
{
    QPoint A1 = {3, 2};
    QPoint B1 = {5, 2};
    QPoint A2 = {2, 2};
    QPoint B2 = {4, 2};

    auto res1 = GeometryRelation::DoesSegmentsCross(A1, B1, A2, B2);
    auto res2 = GeometryRelation::DoesSegmentsCross(A2, B2, A1, B1);

    EXPECT_EQ(res1, true);
    EXPECT_EQ(res2, true);
}

TEST_F(GeomteryRelationsTests, SegmentSegment_Same_Hit)
{
    QPoint A1 = {2, 3};
    QPoint B1 = {2, 5};

    auto res = GeometryRelation::DoesSegmentsCross(A1, B1, A1, B1);

    EXPECT_EQ(res, true);
}

TEST_F(GeomteryRelationsTests, SegmentSegment_Collinear_Miss)
{
    QPoint A1 = {2, 2};
    QPoint B1 = {3, 2};
    QPoint A2 = {4, 2};
    QPoint B2 = {5, 2};

    auto res1 = GeometryRelation::DoesSegmentsCross(A1, B1, A2, B2);
    auto res2 = GeometryRelation::DoesSegmentsCross(A2, B2, A1, B1);

    EXPECT_EQ(res1, false);
    EXPECT_EQ(res2, false);
}

TEST_F(GeomteryRelationsTests, TurnLeft_Typical)
{
    QVector2D A1 = {1, 2};
    QVector2D B1 = {5, 6};
    QVector2D A2 = {5, 3};
    QVector2D B2 = {1, 6};

    auto res1 = GeometryRelation::GetDirectionOnSegmentsTurnLeft(A1, B1, A2, B2);
    auto res2 = GeometryRelation::GetDirectionOnSegmentsTurnLeft(A1, B1, B2, A2);

    EXPECT_EQ(res1, true);
    EXPECT_EQ(res2, false);
}

TEST_F(GeomteryRelationsTests, TurnLeft_Ortogonal)
{
    QVector2D A1 = {3, 2};
    QVector2D B1 = {3, 6};
    QVector2D A2 = {1, 2};
    QVector2D B2 = {6, 2};

    auto res1 = GeometryRelation::GetDirectionOnSegmentsTurnLeft(A1, B1, A2, B2);
    auto res2 = GeometryRelation::GetDirectionOnSegmentsTurnLeft(A1, B1, B2, A2);

    EXPECT_EQ(res1, false);
    EXPECT_EQ(res2, true);
}

TEST_F(GeomteryRelationsTests, TurnRight_Typical)
{
    QVector2D A1 = {1, 2};
    QVector2D B1 = {5, 6};
    QVector2D A2 = {5, 3};
    QVector2D B2 = {1, 6};

    auto res1 = GeometryRelation::GetDirectionOnSegmentsTurnRight(A1, B1, A2, B2);
    auto res2 = GeometryRelation::GetDirectionOnSegmentsTurnRight(A1, B1, B2, A2);

    EXPECT_EQ(res1, false);
    EXPECT_EQ(res2, true);
}