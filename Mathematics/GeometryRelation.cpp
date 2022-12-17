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
                                QVector4D(rect.bottomLeft().x(), rect.topRight().y(), rect.topRight().x(), rect.bottomLeft().y()));
}

bool GeometryRelation::DoesSegmentCrossRect(QVector2D A, QVector2D B, QVector4D rect)
{
    //Cohen-Sutherland
    auto outCodeCompute = [](QVector4D rect, QVector2D p) -> uint
    {
        uint code = 0x0;  // initialised as being inside of clip window

        if (p.x() < rect.x())           // to the left of clip window
            code |= 0x1;
        else if (p.x() > rect.z())      // to the right of clip window
            code |= 0x2;
        if (p.y() < rect.y())           // below the clip window
            code |= 0x4;
        else if (p.y() > rect.w())      // above the clip window
            code |= 0x8;

        return code;
    };

    uint code1 = outCodeCompute(rect, A);
    uint code2 = outCodeCompute(rect, B);

    if (!(code1 | code2))
        return true; //obydwa punkty w srodku prostokata

    if (code1 & code2)
        return false; //obydwa poza prostokatem tak aby na pewno nie bylo przeciecia

    //Jezeli jakies przeciecie istnieje to bedzie przecinac sie teraz z pewnym odcinkiem
    QVector2D LL = {rect.x(), rect.y()};
    QVector2D LU = {rect.x(), rect.w()};
    QVector2D RL = {rect.z(), rect.y()};
    QVector2D RU = {rect.z(), rect.w()};

    if (DoesSegmentsCross(A, B, LL, LU) || DoesSegmentsCross(A, B, LL, RL) ||
        DoesSegmentsCross(A, B, RL, RU) || DoesSegmentsCross(A, B, LU, RU))
        return true;
    return false;
}

bool GeometryRelation::DoesSegmentsCross(QPoint A1, QPoint B1, QPoint A2, QPoint B2)
{
    return DoesSegmentsCross(QVector2D(A1), QVector2D(B1), QVector2D(A2), QVector2D(B2));
}

bool GeometryRelation::DoesSegmentsCross(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
{
    auto res = GetSegmentsCrossPoint(A1, B1, A2, B2);
    if (std::isnan(res.first.x()) || std::isnan(res.first.y()))
        return false;
    return true;
}

std::pair<QVector2D, QVector2D> GeometryRelation::GetSegmentsCrossPoint(QPoint A1, QPoint B1, QPoint A2, QPoint B2)
{
    return GetSegmentsCrossPoint(QVector2D(A1), QVector2D(B1), QVector2D(A2), QVector2D(B2));
}

std::pair<QVector2D, QVector2D> GeometryRelation::GetSegmentsCrossPoint(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2, bool ignoreEndCross)
{
    const float eps = 1e-5;
    QVector2D A1B1 = B1 - A1;
    QVector2D A2B2 = B2 - A2;

    auto cross = [](QVector2D v1, QVector2D v2) -> float
    {
        return v1.x() * v2.y() - v1.y() * v2.x();
    };

    float rcs = cross(A1B1, A2B2);
    float qpcr = cross(A2 - A1, A1B1);
    if (abs(rcs) < eps && abs(qpcr) < eps)
    {
        //Przypadek wspolliniowowsci
        float t1 = QVector2D::dotProduct((A2 - A1), A1B1) / QVector2D::dotProduct(A1B1, A1B1);
        float t2 = t1 + QVector2D::dotProduct(A2B2, A1B1) / QVector2D::dotProduct(A1B1, A1B1);

        if ((t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1))
            return std::make_pair(A1 + t1 * A1B1, QVector2D(t1, t2));//cokolwiek
        return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {NAN, NAN});
    }

    if (abs(rcs) < 1e-5)
        return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {NAN, NAN});//rownolegle i nie przecinajace sie

    float t1 = cross((A2 - A1), A2B2) / rcs;
    float t2 = cross((A2 - A1), A1B1) / rcs;

    if (ignoreEndCross)
    {
        if (t1 > eps && t1 < 1 - eps && t2 > eps && t2 < 1 - eps)
            return std::make_pair(A1 + t1 * A1B1, QVector2D(t1, t2));
    }
    else
    {
        if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
            return std::make_pair(A1 + t1 * A1B1, QVector2D(t1, t2));
    }
    return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {t1, t2});
}

bool GeometryRelation::GetDirectionOnSegmentsTurnLeft(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
{
    QVector3D fromVec, ascVec;
    //Wektory 3D z zerowym Z
    fromVec = QVector3D(B1 - A1);
    ascVec = QVector3D(B2 - A1);

    //Cross w strone rosnaca wyszedl dodatni => skrecamy w lewo do startu
    if (QVector3D::crossProduct(fromVec, ascVec).z() > 0)
        return true;//bedziemy isc w przod kolejnej lamanej
    //Cross w strone rosnaca wyszedl ujemny => skrecamy w prawo do startu, czyli w lewo do Endu
    return false;//bedziemy isc w tyl kolejnej lamanej
}

bool GeometryRelation::GetDirectionOnSegmentsTurnRight(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
{
    return !GetDirectionOnSegmentsTurnLeft(A1, B1, A2, B2);
}
