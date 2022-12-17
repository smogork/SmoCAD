//
// Created by ksm on 12/2/22.
//

#ifndef SMOCAD_PLANEDIVISION_H
#define SMOCAD_PLANEDIVISION_H

#include <QVector4D>
#include <list>
#include "GeometryRelation.h"

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
        bool WrapsX = false;

        PolylineSegment(QVector2D start, QVector2D end, int polylineIdx, int segmentIdx)
                : Start(start), End(end), PolylineIndex(polylineIdx), SegmentIndex(segmentIdx)
        {}

        /*PolylineSegment(const PolylineSegment& other)
        {
            Start = other.Start;
            End = other.End;
            PolylineIndex = other.PolylineIndex;
            SegmentIndex = other.SegmentIndex;
            InsideDivisions.clear();
            std::copy(other.InsideDivisions.begin(), other.InsideDivisions.end(), InsideDivisions.begin());
            WrapsX = other.WrapsX;
        }*/


        void CheckForWrapX(QVector4D planeSize)
        {
            float jumpX = std::abs(End.x() - Start.x());

            // Jezeli przeskok wspolrzednej na X jest wiekszy niz polowa szerokosci plaszczyzny -> zapetlilismy sie
            if (jumpX > (planeSize.z() - planeSize.x()) / 2.0f)
                WrapsX = true;
        }

        PolylineSegment WrapedSegmentOne(QVector4D planeSize) const
        {
            float midX = (planeSize.z() + planeSize.x()) / 2.0f;
            QVector2D xOffset(planeSize.z() - planeSize.x(), 0.0f);

            if (Start.x() > midX)
                return PolylineSegment(Start, End + xOffset, PolylineIndex, SegmentIndex);
            return PolylineSegment(Start, End - xOffset, PolylineIndex, SegmentIndex);

        }

        PolylineSegment WrapedSegmentTwo(QVector4D planeSize) const
        {
            float midX = (planeSize.z() + planeSize.x()) / 2.0f;
            QVector2D xOffset(planeSize.z() - planeSize.x(), 0.0f);

            if (Start.x() > midX)
                return PolylineSegment(Start - xOffset, End, PolylineIndex, SegmentIndex);
            return PolylineSegment(Start + xOffset, End, PolylineIndex, SegmentIndex);
        }

        bool CrossesWithRect(QVector4D rect, QVector4D planeSize)
        {
            if (!WrapsX)
                return GeometryRelation::DoesSegmentCrossRect(Start, End, rect);

            auto one = WrapedSegmentOne(planeSize);
            auto two = WrapedSegmentTwo(planeSize);
            return one.CrossesWithRect(rect, planeSize) ||
                    two.CrossesWithRect(rect, planeSize);
        }

        std::pair<QVector2D, QVector2D> GetCrossPointWith(const PolylineSegment& other, QVector4D planeSize, bool ignoreEnds = false)
        {
            if (!WrapsX && !other.WrapsX)
                return GeometryRelation::GetSegmentsCrossPoint(Start, End, other.Start, other.End, ignoreEnds);

            if (WrapsX && !other.WrapsX)
            {
                auto one = WrapedSegmentOne(planeSize);
                auto two = WrapedSegmentTwo(planeSize);

                auto resOne = one.GetCrossPointWith(other, planeSize, ignoreEnds);
                if (!std::isnan(resOne.first.x()) && !std::isnan(resOne.first.y()))
                    return resOne;

                auto resTwo = two.GetCrossPointWith(other, planeSize, ignoreEnds);
                if (!std::isnan(resTwo.first.x()) && !std::isnan(resTwo.first.y()))
                    return resTwo;

                return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {NAN, NAN});
            }

            if (!WrapsX && other.WrapsX)
            {
                auto one = other.WrapedSegmentOne(planeSize);
                auto two = other.WrapedSegmentTwo(planeSize);

                auto resOne = one.GetCrossPointWith(*this, planeSize, ignoreEnds);
                if (!std::isnan(resOne.first.x()) && !std::isnan(resOne.first.y()))
                    return resOne;

                auto resTwo = two.GetCrossPointWith(*this, planeSize, ignoreEnds);
                if (!std::isnan(resTwo.first.x()) && !std::isnan(resTwo.first.y()))
                    return resTwo;

                return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {NAN, NAN});;
            }

            auto oneT = other.WrapedSegmentOne(planeSize);
            auto twoT = other.WrapedSegmentTwo(planeSize);
            auto oneO = other.WrapedSegmentOne(planeSize);
            auto twoO = other.WrapedSegmentTwo(planeSize);

            auto resOne = oneT.GetCrossPointWith(oneO, planeSize, ignoreEnds);
            if (!std::isnan(resOne.first.x()) && !std::isnan(resOne.first.y()))
                return resOne;

            auto resTwo = oneT.GetCrossPointWith(twoO, planeSize, ignoreEnds);
            if (!std::isnan(resTwo.first.x()) && !std::isnan(resTwo.first.y()))
                return resTwo;

            auto resThird = twoT.GetCrossPointWith(oneO, planeSize, ignoreEnds);
            if (!std::isnan(resThird.first.x()) && !std::isnan(resThird.first.y()))
                return resThird;

            auto resFour = twoT.GetCrossPointWith(twoO, planeSize, ignoreEnds);
            if (!std::isnan(resFour.first.x()) && !std::isnan(resFour.first.y()))
                return resFour;

            return std::make_pair<QVector2D, QVector2D>({NAN, NAN}, {NAN, NAN});
        }

        std::pair<float, float> GetMinMaxX(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2)
        {
            std::vector<float> data {
                A1.x(), B1.x(), A2.x(), B2.x()
            };

            float mini = *std::min(data.begin(), data.end());
            float maxi = *std::max(data.begin(), data.end());
            return std::make_pair(mini, maxi);
        }

        bool GetDirectionOnTurnLeft(const PolylineSegment& other, QVector4D planeSize, bool fromAscending)
        {
            if (!WrapsX && !other.WrapsX)
            {
                if (!fromAscending)
                    return GeometryRelation::GetDirectionOnSegmentsTurnLeft(End, Start, other.Start, other.End);
                return GeometryRelation::GetDirectionOnSegmentsTurnLeft(Start, End, other.Start, other.End);
            }

            float halfWidth = (planeSize.z() - planeSize.x()) / 2.0f;
            if (WrapsX && !other.WrapsX)
            {
                auto one = WrapedSegmentOne(planeSize);
                auto two = WrapedSegmentTwo(planeSize);

                auto minmaxOne = GetMinMaxX(one.Start, one.End, other.Start, other.End);
                if (minmaxOne.second - minmaxOne.first < halfWidth)
                    return one.GetDirectionOnTurnLeft(other, planeSize, fromAscending);

                auto minmaxTwo = GetMinMaxX(two.Start, two.End, other.Start, other.End);
                if (minmaxOne.second - minmaxOne.first < halfWidth)
                    return one.GetDirectionOnTurnLeft(other, planeSize, fromAscending);
            }

            if (!WrapsX && other.WrapsX)
            {
                auto one = other.WrapedSegmentOne(planeSize);
                auto two = other.WrapedSegmentTwo(planeSize);

                auto minmaxOne = GetMinMaxX(Start, End, one.Start, one.End);
                if (minmaxOne.second - minmaxOne.first < halfWidth)
                    return GetDirectionOnTurnLeft(one, planeSize, fromAscending);

                auto minmaxTwo = GetMinMaxX(Start, End, two.Start, two.End);
                if (minmaxOne.second - minmaxOne.first < halfWidth)
                    return GetDirectionOnTurnLeft(two, planeSize, fromAscending);
            }

            auto oneT = other.WrapedSegmentOne(planeSize);
            auto twoT = other.WrapedSegmentTwo(planeSize);
            auto oneO = other.WrapedSegmentOne(planeSize);
            auto twoO = other.WrapedSegmentTwo(planeSize);

            auto minmaxOne = GetMinMaxX(oneT.Start, oneT.End, oneO.Start, oneO.End);
            if (minmaxOne.second - minmaxOne.first < halfWidth)
                return oneT.GetDirectionOnTurnLeft(oneO, planeSize, fromAscending);

            auto minmaxTwo = GetMinMaxX(oneT.Start, oneT.End, twoO.Start, twoO.End);
            if (minmaxOne.second - minmaxOne.first < halfWidth)
                return oneT.GetDirectionOnTurnLeft(twoO, planeSize, fromAscending);

            auto minmaxThree = GetMinMaxX(twoT.Start, twoT.End, oneO.Start, oneO.End);
            if (minmaxThree.second - minmaxThree.first < halfWidth)
                return twoT.GetDirectionOnTurnLeft(oneO, planeSize, fromAscending);

            auto minmaxFour = GetMinMaxX(twoT.Start, twoT.End, twoO.Start, twoO.End);
            if (minmaxFour.second - minmaxFour.first < halfWidth)
                return twoT.GetDirectionOnTurnLeft(twoO, planeSize, fromAscending);
        }

        bool GetDirectionOnTurnRight(const PolylineSegment& other, QVector4D planeSize, bool fromAscending)
        {
            return !GetDirectionOnTurnLeft(other, planeSize, fromAscending);
        }
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
        PolylineSegment* From;
        PolylineSegment* To;
        float FromT;
        float ToT;
        Division *IntersectionDivision;
        QVector2D CrossPoint;

        PolylineCross(PolylineSegment* from, PolylineSegment* to, float fromT, float toT, Division *div, QVector2D crossPoint) :
                From(from), To(to), IntersectionDivision(div), CrossPoint(crossPoint), FromT(fromT), ToT(toT)
        {}

        /*PolylineCross(PolylineCross &other): From(other.From), To(other.To)
        {
            IntersectionDivision = other.IntersectionDivision;
            CrossPoint = other.CrossPoint;
        }*/
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

    PolylineCross
    GetFirstIntersectFrom(bool direction, int polylineIdx, int segmentIdx, std::vector<QVector2D> &passedPoints, float skipBeforeT);

public:
    bool DebugImages = false;
    bool WrapX = false;
    float Width() const;
    float Height() const;
    QVector4D PlaneSize() const;

    explicit PlaneDivision(QVector4D planeSize);
    ~PlaneDivision();

    void AddConstraintPolyline(const std::vector<QVector2D> &points, bool cycle = false);
    void AddConstraintPolyline(const std::vector<QVector3D> &points, bool cycle = false);
    int GetConstraintCount();

    /**
     *
     * @param planeSize Rozmiar plaszczyzny w postaci lewego dolnego naroznika oraz prawego gornego naroznika
     * @param divisionCount Liczba podzialow w jednej osi
     */
    void CreateDivision(int divisionCount = 64);
    std::vector<QVector2D> JoinConstraintPolylinesTogetherInCycle(int startPolylineIndex = 0);
    std::vector<QVector2D>
    JoinConstraintPolylinesZigzag(std::vector<int> polylineToVisit, std::vector<int> switchTurnPolylines, bool startAscending, bool firstTurnRight,
                                  int startPolylineIndex = 0, int startPolylineSegment = 0);

};

#endif //SMOCAD_PLANEDIVISION_H
