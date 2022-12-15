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
    struct PolylineSegment
    {
        QVector2D Start;
        QVector2D End;
        int PolylineIndex;
        int SegmentIndex;
        std::list<std::pair<int, int>> InsideDivisions;

        PolylineSegment(QVector2D start, QVector2D end, int polylineIdx, int segmentIdx)
        : Start(start), End(end), PolylineIndex(polylineIdx), SegmentIndex(segmentIdx)
        { }
    };

    struct Division
    {
        QVector2D Start;
        QVector2D End;
        std::list<PolylineSegment*> SegmentsInside;

        Division() {}
        Division(QVector2D start, QVector2D end, int polylineIndex, int startIndex)
        : Start(start), End(end)
        { }

        QVector4D GetRect() const
        {
            return {Start.x(), Start.y(), End.x(), End.y()};
        }
    };

    //{startX, startY, endX, endY} + index of polyline owning segment
    Division* m_division;
    std::vector<std::vector<PolylineSegment>> m_polylines;
    int m_size = 0;
    bool m_divisionCreated = false;
    QVector4D m_planeSize;

    Division& GetDivision(int col, int row);
    void DebugImageOfPlane();

    bool GetDirectionOnPolylineOuterChange(const PolylineSegment& from, const PolylineSegment& to);

public:
    bool WrapX = false;
    float Width() const;
    float Height() const;
    QVector4D PlaneSize() const;

    explicit PlaneDivision(QVector4D planeSize);
    ~PlaneDivision();

    void AddConstraintPolyline(const std::vector<QVector2D>& points, bool flip = false);
    void AddConstraintPolyline(const std::vector<QVector3D>& points, bool flip = false);

    /**
     *
     * @param planeSize Rozmiar plaszczyzny w postaci lewego dolnego naroznika oraz prawego gornego naroznika
     * @param divisionCount Liczba podzialow w jednej osi
     */
    void CreateDivision(int divisionCount = 64);
    std::vector<QVector2D> JoinConstraintPolylinesTogether(int startPolylineIndex = 0);

};

#endif //SMOCAD_PLANEDIVISION_H
