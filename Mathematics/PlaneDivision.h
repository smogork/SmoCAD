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
        {}
    };

    struct Division
    {
        QVector2D Start;
        QVector2D End;
        std::list<PolylineSegment *> SegmentsInside;

        Division()
        {}

        Division(QVector2D start, QVector2D end, int polylineIndex, int startIndex)
                : Start(start), End(end)
        {}

        QVector4D GetRect() const
        {
            return {Start.x(), Start.y(), End.x(), End.y()};
        }
    };

    struct PolylineCross
    {
        PolylineSegment *From;
        PolylineSegment *To;
        Division *IntersectionDivision;
        QVector2D CrossPoint;

        PolylineCross(PolylineSegment *from, PolylineSegment *to, Division *div, QVector2D crossPoint) :
                From(from), To(to), IntersectionDivision(div), CrossPoint(crossPoint)
        {}

        PolylineCross(PolylineCross &other)
        {
            From = other.From;
            To = other.To;
            IntersectionDivision = other.IntersectionDivision;
            CrossPoint = other.CrossPoint;
        }
    };

    class CrossPolylineMissingException : std::runtime_error
    {
    public:
        QVector2D LastPoint;

        CrossPolylineMissingException(QVector2D lastPoint) : std::runtime_error(""), LastPoint(lastPoint)
        {}
    };

    Division *m_division;
    std::vector<std::vector<PolylineSegment>> m_polylines;
    int m_size = 0;
    bool m_divisionCreated = false;
    QVector4D m_planeSize;

    Division &GetDivision(int col, int row);
    void DebugImageOfPlane();

    bool GetDirectionOnPolylineLeftTurn(const PolylineSegment &from, const PolylineSegment &to, bool fromAscending);
    bool GetDirectionOnPolylineRightTurn(const PolylineSegment &from, const PolylineSegment &to, bool fromAscending);
    PolylineCross
    GetFirstIntersectFrom(bool direction, int polylineIdx, int segmentIdx, std::vector<QVector2D> &passedPoints);

public:
    bool WrapX = false;
    float Width() const;
    float Height() const;
    QVector4D PlaneSize() const;

    explicit PlaneDivision(QVector4D planeSize);
    ~PlaneDivision();

    void AddConstraintPolyline(const std::vector<QVector2D> &points, bool flip = false);
    void AddConstraintPolyline(const std::vector<QVector3D> &points, bool flip = false);
    int GetConstraintCount();

    /**
     *
     * @param planeSize Rozmiar plaszczyzny w postaci lewego dolnego naroznika oraz prawego gornego naroznika
     * @param divisionCount Liczba podzialow w jednej osi
     */
    void CreateDivision(int divisionCount = 64);
    std::vector<QVector2D> JoinConstraintPolylinesTogether(int startPolylineIndex = 0);
    std::vector<QVector2D>
    JoinConstraintPolylinesZigzag(std::vector<int> polylineToVisit, bool startAscending, bool firstTurnRight,
                                  int startPolylineIndex = 0, int startPolylineSegment = 0);

};

#endif //SMOCAD_PLANEDIVISION_H
