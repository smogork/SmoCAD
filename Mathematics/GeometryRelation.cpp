//
// Created by ksm on 6/11/22.
//

#include "GeometryRelation.h"

bool GeometryRelation::IsPointInsideRect(QPoint p, QRect rect)
{
    return rect.contains(p);
}

bool GeometryRelation::DoesSegmentCrossRect(QPoint A, QPoint B, QRect rect)
{
    return DoesSegmentCrossRect(QVector2D(A), QVector2D(B),
                                QVector4D(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height()));
}

bool GeometryRelation::DoesSegmentCrossRect(QVector2D A, QVector2D B, QVector4D rect)
{
    QVector2D LL = {rect.x(), rect.y()};
    QVector2D LU = {rect.x(), rect.w()};
    QVector2D RL = {rect.z(), rect.y()};
    QVector2D RU = {rect.z(), rect.w()};

    //Funkcja prostej w postaci implicit
    QVector2D lineDir = QVector2D((B-A).y(), (A-B).x()).normalized();//prostopadly wektor do prostej
    auto evaluateLine = [](QVector2D lineDir, QVector2D p) -> float
    {
        return QVector2D::dotProduct(lineDir, p);
    };

    float t1 = evaluateLine(lineDir, LL);
    float t2 = evaluateLine(lineDir, LU);
    float t3 = evaluateLine(lineDir, RL);
    float t4 = evaluateLine(lineDir, RU);

    //https://stackoverflow.com/a/2368338
    //Jezeli ewalucja kazdego naroznika wyszla po jednej stronie prostej => brak przeciecia
    if ((t1 <= 0 && t2 <= 0 && t3 <= 0 && t4 <= 0) || (t1 >= 0 && t2 >= 0 && t3 >= 0 && t4 >= 0))
        return false;
    return true;

    //To alternatywne podejscie tez zadziala. To wyzej jest szybsze
    /*if (DoesSegmentsCross(A, B, LL, LU) || DoesSegmentsCross(A, B, LL, RL) ||
        DoesSegmentsCross(A, B, RL, RU) || DoesSegmentsCross(A, B, LU, RU))
        return true;
    return false;*/
}

bool GeometryRelation::DoesSegmentsCross(QPoint A1, QPoint B1, QPoint A2, QPoint B2)
{
    return DoesSegmentsCross(QVector2D(A1), QVector2D(B1), QVector2D(A2), QVector2D(B2));
}

bool GeometryRelation::DoesSegmentsCross(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
{
    auto res = GetSegmentsCrossPoint(A1, B1, A2, B2);
    if (std::isnan(res.x()) || std::isnan(res.y()))
        return false;
    return true;
}

QVector2D GeometryRelation::GetSegmentsCrossPoint(QPoint A1, QPoint B1, QPoint A2, QPoint B2)
{
    return GetSegmentsCrossPoint(QVector2D(A1), QVector2D(B1), QVector2D(A2), QVector2D(B2));
}

QVector2D GeometryRelation::GetSegmentsCrossPoint(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
{
    QVector2D A1B1 = B1 - A1;
    QVector2D A2B2 = B2 - A2;
    QVector2D A1A2 = A2 - A1;

    float t1 = (A2B2.x()*A1A2.y() - A2B2.y()*A1A2.x()) / (A2B2.y()*A1B1.x() - A2B2.x()*A1B1.y() );
    float t2 = (A1B1.x()*A1A2.y() - A1B1.y()*A1A2.x()) / (A2B2.y()*A1B1.x() - A2B2.x()*A1B1.y());

    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
        return A1 + t1 * A1B1;
    return {NAN, NAN};
}
