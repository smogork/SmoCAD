//
// Created by ksm on 12/2/22.
//

#include <QImage>
#include <QPainter>
#include "PlaneDivision.h"
#include "GeometryRelation.h"
#include <set>

PlaneDivision::PlaneDivision(QVector4D planeSize) : m_planeSize(planeSize)
{

}

PlaneDivision::~PlaneDivision()
{
    if (m_divisionCreated)
        delete[] m_division;
}

PlaneDivision::Division &PlaneDivision::GetDivision(int col, int row)
{
    return m_division[row * m_size + col];
}

void PlaneDivision::AddConstraintPolyline(const std::vector<QVector2D> &points, bool cycle)
{
    std::vector<PolylineSegment> insidePoly;
    int counter = 0;
    for (int i = 1; i < points.size(); ++i)
    {
        PolylineSegment seg(points[i - 1], points[i], m_polylines.size(), counter++);

        if (WrapX)
            seg.CheckForWrapX(m_planeSize);

        insidePoly.emplace_back(seg);
    }

    if (cycle)
    {
        PolylineSegment seg(points.back(), points.front(), m_polylines.size(), counter++);
        if (WrapX)
            seg.CheckForWrapX(m_planeSize);
        insidePoly.emplace_back(seg);
    }

    m_polylines.emplace_back(insidePoly);
}

void PlaneDivision::CreateDivision(int divisionCount)
{
    m_size = divisionCount;
    m_division = new Division[m_size * m_size];

    float dx = (m_planeSize.z() - m_planeSize.x()) / m_size;
    float dy = (m_planeSize.w() - m_planeSize.y()) / m_size;

    for (int y = 0; y < m_size; ++y)
        for (int x = 0; x < m_size; ++x)
        {
            Division &curDiv = GetDivision(x, y);
            curDiv.Start = {m_planeSize.x() + dx * x, m_planeSize.y() + dy * y};
            curDiv.End = curDiv.Start + QVector2D(dx, dy);
            auto rect = curDiv.GetRect();

            for (int j = 0; j < m_polylines.size(); ++j)
            {
                for (PolylineSegment &seg: m_polylines[j])
                {
                    if (seg.CrossesWithRect(rect, m_planeSize))
                    {
                        curDiv.SegmentsInside.emplace_back(&seg);
                        seg.InsideDivisions.emplace_back(std::make_pair(x, y));
                    }
                }
            }
        }

    m_divisionCreated = true;
    if (DebugImages)
        DebugImageOfPlane();
}

std::vector<QVector2D> PlaneDivision::JoinConstraintPolylinesTogetherInCycle(int startPolylineIndex)
{
    std::vector<QVector2D> resultPolyline;

    int polylineIdx = startPolylineIndex;
    int segmentIdx = 0;
    bool segmentIdxAscending = true;
    do
    {
        try
        {
            auto intersection = GetFirstIntersectFrom(segmentIdxAscending, polylineIdx, segmentIdx, resultPolyline);
            resultPolyline.emplace_back(intersection.CrossPoint);

            segmentIdxAscending = intersection.From->GetDirectionOnTurnRight(*intersection.To, m_planeSize, segmentIdxAscending);
            polylineIdx = intersection.To->PolylineIndex;
            segmentIdx = intersection.To->SegmentIndex + (segmentIdxAscending ? 1 : -1);

        }
        catch (CrossPolylineMissingException &e)//Obsluga braku przeciecia
        {
            auto poly = m_polylines[polylineIdx];
            resultPolyline.emplace_back(e.LastPoint);

            if (segmentIdxAscending)//doszliśmy do końca
            {
                if (poly.front().Start != poly.back().End)//w przypadku niecyklicznej lamanej przechodzimy do innej
                    polylineIdx = (polylineIdx + 1) % m_polylines.size();
                segmentIdx = 0;
            }
            else
            {
                if (poly.front().Start != poly.back().End)//w przypadku niecyklicznej lamanej przechodzimy do innej
                    polylineIdx = polylineIdx == 0 ? m_polylines.size() - 1 : polylineIdx - 1;
                segmentIdx = m_polylines[polylineIdx].size() - 1;
            }
        }


    } while ((segmentIdx != 0 || polylineIdx != startPolylineIndex)
             && polylineIdx < m_polylines.size() && polylineIdx >= 0);//Wróciliśmy do początku

    return resultPolyline;
}

void PlaneDivision::DebugImageOfPlane()
{
    if (!m_divisionCreated)
        return;
    //16 pixeli na klatke podzialu
    QImage plane(QSize(16 * m_size, 16 * m_size), QImage::Format_ARGB32);

    auto fromPlaneToImage = [this, &plane](QVector2D planePoint) -> QPoint
    {
        float widthX = m_planeSize.z() - m_planeSize.x();
        float widthY = m_planeSize.w() - m_planeSize.y();

        return QPoint(
                ((planePoint.x() - m_planeSize.x()) / widthX) * plane.width(),
                (1.0f - ((planePoint.y() - m_planeSize.y()) / widthY)) * plane.height()
        );
    };

    QPainter p(&plane);
    p.fillRect(QRect(0, 0, plane.width(), plane.height()), Qt::black);
    p.setPen(Qt::darkGray);

    //Narysuj kratki podzialu
    /*float dx = (m_planeSize.z() - m_planeSize.x()) / m_size;
    float dy = (m_planeSize.w() - m_planeSize.y()) / m_size;
    for (int y = 0; y < m_size; ++y)
        for (int x = 0; x < m_size; ++x)
        {
            QVector2D leftLower = {m_planeSize.x() + dx * x, m_planeSize.y() + dy * y};
            QVector2D rightUpper = leftLower + QVector2D(dx, dy);
            QRect r(fromPlaneToImage(leftLower), fromPlaneToImage(rightUpper));
            //p.drawRect(r);
        }*/

    //Narysuj linie lamanych
    Qt::GlobalColor polylineColor = Qt::white;
    for (const auto &polyline: m_polylines)
    {
        p.setPen(polylineColor);
        for (const PolylineSegment &seg: polyline)
        {
            if (seg.WrapsX)
            {
                auto one = seg.WrapedSegmentOne(m_planeSize);
                auto two = seg.WrapedSegmentTwo(m_planeSize);

                QPoint A = fromPlaneToImage(one.Start);
                QPoint B = fromPlaneToImage(one.End);
                A.setX(std::clamp(A.x(), 0, plane.width() - 1));
                A.setY(std::clamp(A.y(), 0, plane.height() - 1));
                B.setX(std::clamp(B.x(), 0, plane.width() - 1));
                B.setY(std::clamp(B.y(), 0, plane.height() - 1));
                p.drawLine(A, B);

                QPoint C = fromPlaneToImage(two.Start);
                QPoint D = fromPlaneToImage(two.End);
                C.setX(std::clamp(C.x(), 0, plane.width() - 1));
                C.setY(std::clamp(C.y(), 0, plane.height() - 1));
                D.setX(std::clamp(D.x(), 0, plane.width() - 1));
                D.setY(std::clamp(D.y(), 0, plane.height() - 1));
                p.drawLine(C, D);
            }
            else
            {
                QPoint A = fromPlaneToImage(seg.Start);
                QPoint B = fromPlaneToImage(seg.End);
                A.setX(std::clamp(A.x(), 0, plane.width() - 1));
                A.setY(std::clamp(A.y(), 0, plane.height() - 1));
                B.setX(std::clamp(B.x(), 0, plane.width() - 1));
                B.setY(std::clamp(B.y(), 0, plane.height() - 1));
                p.drawLine(A, B);
            }

        }
        polylineColor = (Qt::GlobalColor) (polylineColor + 1);
    }

    //narysuj poczatki i konce lamanych
    for (const auto &polyline: m_polylines)
    {
        if (polyline.empty())
            continue;

        QPoint S = fromPlaneToImage(polyline.front().Start);
        QPoint E = fromPlaneToImage(polyline.back().End);
        QPoint PointSize = {3, 3};

        p.fillRect(QRect(S - PointSize, S + PointSize), Qt::magenta);
        p.fillRect(QRect(E - PointSize, E + PointSize), Qt::yellow);
    }

    plane.save("DivisionDebug.png");
}

void PlaneDivision::AddConstraintPolyline(const std::vector<QVector3D> &points, bool cycle)
{
    std::vector<QVector2D> planePoints;
    planePoints.reserve(points.size());
    for (const auto &p: points)
        planePoints.emplace_back(QVector2D(p.x(), p.z()));
    AddConstraintPolyline(planePoints, cycle);
}


float PlaneDivision::Width() const
{
    return m_planeSize.z() - m_planeSize.x();
}

float PlaneDivision::Height() const
{
    return m_planeSize.w() - m_planeSize.y();
}

QVector4D PlaneDivision::PlaneSize() const
{
    return m_planeSize;
}

int PlaneDivision::GetConstraintCount()
{
    return m_polylines.size();
}


std::vector<QVector2D>
PlaneDivision::JoinConstraintPolylinesZigzag(std::vector<int> polylineToVisit, std::vector<int> switchTurnPolylines, bool startAscending, bool firstTurnRight,
                                             int startPolylineIndex, int startPolylineSegment)
{
    int polylineIdx = startPolylineIndex;
    int segmentIdx = startPolylineSegment;
    bool segmentIdxAscending = startAscending;
    bool turnRight = firstTurnRight;

    std::vector<QVector2D> resultPolyline;
    std::set<int> toVisit;
    std::set<int> switchTurn;

    for (int p: polylineToVisit)
        toVisit.insert(p);

    for (int p : switchTurnPolylines)
        switchTurn.insert(p);

    const int itLimit = 50 * polylineToVisit.size();
    int it = 0;
    do
    {
        try
        {
            auto intersection = GetFirstIntersectFrom(segmentIdxAscending, polylineIdx, segmentIdx, resultPolyline);
            resultPolyline.emplace_back(intersection.CrossPoint);

            if (DebugImages)
            {
                PlaneDivision debug(m_planeSize);
                debug.DebugImages = true;
                debug.AddConstraintPolyline(resultPolyline);
                debug.CreateDivision();
            }

            //Oznaczenie jako odwiedzonego wielomianu, z ktroego schodzimy
            toVisit.erase(polylineIdx);

            if (turnRight)
                segmentIdxAscending = intersection.From->GetDirectionOnTurnRight(*intersection.To, m_planeSize, segmentIdxAscending);
            else
                segmentIdxAscending = intersection.From->GetDirectionOnTurnLeft(*intersection.To, m_planeSize, segmentIdxAscending);

            auto poly = m_polylines[polylineIdx];
            if (switchTurn.contains(intersection.From->PolylineIndex) && !switchTurn.contains(intersection.To->PolylineIndex))
                turnRight = !turnRight;

            polylineIdx = intersection.To->PolylineIndex;
            segmentIdx = intersection.To->SegmentIndex + (segmentIdxAscending ? 1 : -1);

        }
        catch (CrossPolylineMissingException &e)//Obsluga braku przeciecia
        {
            //Oznaczenie jako odwiedzonego wielomianu, z ktroego schodzimy
            toVisit.erase(polylineIdx);
            auto poly = m_polylines[polylineIdx];
            resultPolyline.emplace_back(e.LastPoint);

            if (segmentIdxAscending)//doszliśmy do końca
            {
                if (poly.front().Start != poly.back().End)//w przypadku niecyklicznej lamanej przechodzimy do innej
                    polylineIdx = (polylineIdx + 1) % m_polylines.size();
                segmentIdx = 0;
            }
            else
            {
                if (poly.front().Start != poly.back().End)//w przypadku niecyklicznej lamanej przechodzimy do innej
                    polylineIdx = polylineIdx == 0 ? m_polylines.size() - 1 : polylineIdx - 1;
                segmentIdx = m_polylines[polylineIdx].size() - 1;
            }
        }



        it++;
    } while (!toVisit.empty() && it < itLimit);//odwiedzilismy wszystkie okreslone wielomiany

    return resultPolyline;
}

PlaneDivision::PolylineCross
PlaneDivision::GetFirstIntersectFrom(bool direction, int polylineIdx, int segmentIdx,
                                     std::vector<QVector2D> &passedPoints)
{
    auto poly = m_polylines[polylineIdx];
    bool isCycle = poly.front().Start == poly.back().End;
    int idx = segmentIdx;
    if (isCycle)
    {
        if (idx < 0)
            idx += poly.size();
        else if (idx >= poly.size())
            idx -= poly.size();
    }
    idx = std::clamp(idx, 0, (int)poly.size() - 1);
    PolylineSegment seg = poly[idx];



    int segmentCounter = 0;
    while ((!isCycle && idx < poly.size() && idx >= 0) || (isCycle && segmentCounter < poly.size()))
    {
        seg = poly[idx];
        passedPoints.emplace_back(direction ? seg.Start : seg.End);

        //Sprawdzmy czy rozwazany segment nie przecina sie z innymi
        QVector2D crossPoint = {NAN, NAN};
        for (const auto divIdx: seg.InsideDivisions)
        {
            Division &curDiv = GetDivision(divIdx.first, divIdx.second);
            for (PolylineSegment *divSeg: curDiv.SegmentsInside)
            {
                if (divSeg->PolylineIndex == seg.PolylineIndex)
                    continue;

                crossPoint = seg.GetCrossPointWith(*divSeg, m_planeSize);

                //Jezelie znalezlismy przecięcie to koniec
                if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
                {
                    return PlaneDivision::PolylineCross(&m_polylines[polylineIdx][idx], divSeg, &curDiv, crossPoint);
                }
            }
        }


        idx += (direction ? 1 : -1);
        if (isCycle)
        {
            if (idx < 0)
                idx += poly.size();
            else if (idx >= poly.size())
                idx -= poly.size();
        }
    }

    throw CrossPolylineMissingException(direction ? seg.End : seg.Start);
}

