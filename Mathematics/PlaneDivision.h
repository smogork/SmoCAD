//
// Created by ksm on 12/2/22.
//

#ifndef SMOCAD_PLANEDIVISION_H
#define SMOCAD_PLANEDIVISION_H

#include <QVector4D>
#include <list>

class PlaneDivision
{
private:
    struct ConstraintSegment
    {
        QVector2D Start;
        QVector2D End;
        int PolylineIndex;
        int StartIndex;

        ConstraintSegment(QVector2D start, QVector2D end, int polylineIndex, int startIndex)
        : Start(start), End(end), PolylineIndex(polylineIndex), StartIndex(startIndex)
        { }
    };

    //{startX, startY, endX, endY} + index of polyline owning segment
    std::list<ConstraintSegment>* m_division;
    std::vector<std::vector<QVector2D>> m_polylines;
    int m_size = 0;
    bool m_divisionCreated = false;

    std::list<ConstraintSegment>& GetDivision(int col, int row);
    void DebugImageOfPlane(QVector4D planeSize);

public:
    PlaneDivision();
    ~PlaneDivision();

    void AddConstraintPolyline(const std::vector<QVector2D>& points, bool flip = false);
    void AddConstraintPolyline(const std::vector<QVector3D>& points, bool flip = false);
    std::vector<QVector2D>& GetConstraintPolyline(int index);

    /**
     *
     * @param planeSize Rozmiar plaszczyzny w postaci lewego dolnego naroznika oraz prawego gornego naroznika
     * @param divisionCount Liczba podzialow w jednej osi
     */
    void CreateDivision(QVector4D planeSize, int divisionCount = 64);
    std::vector<QVector2D> JoinConstraintPolylinesTogether(int startPolylineIndex = 0);

};

#endif //SMOCAD_PLANEDIVISION_H
