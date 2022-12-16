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

void PlaneDivision::AddConstraintPolyline(const std::vector<QVector2D> &points, bool flip)
{
    std::vector<PolylineSegment> insidePoly;
    int counter = 0;
    if (flip)
    {
        for (int i = points.size() - 1; i >= 1; --i)
            insidePoly.emplace_back(PolylineSegment(points[i], points[i - 1], m_polylines.size(), counter++));
    } else
    {
        for (int i = 1; i < points.size(); ++i)
            insidePoly.emplace_back(PolylineSegment(points[i - 1], points[i], m_polylines.size(), counter++));
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
                    if (GeometryRelation::DoesSegmentCrossRect(seg.Start, seg.End, rect))
                    {
                        curDiv.SegmentsInside.emplace_back(&seg);
                        seg.InsideDivisions.emplace_back(std::make_pair(x, y));
                    }
                }
            }
        }

    m_divisionCreated = true;
    DebugImageOfPlane();
}

std::vector<QVector2D> PlaneDivision::JoinConstraintPolylinesTogether(int startPolylineIndex)
{
    std::vector<QVector2D> resultPolyline;

    int polylineIdx = startPolylineIndex;
    int segmentIdx = 0;
    bool segmentIdxAscending = true;
    do
    {
        PolylineSegment &seg = m_polylines[polylineIdx][segmentIdx];
        resultPolyline.emplace_back(segmentIdxAscending ? seg.Start : seg.End);

        //Sprawdzmy czy rozwazany segment nie przecina sie z innymi
        QVector2D crossPoint = {NAN, NAN};
        for (const auto divIdx: seg.InsideDivisions)
        {
            Division &curDiv = GetDivision(divIdx.first, divIdx.second);
            for (PolylineSegment *divSeg: curDiv.SegmentsInside)
            {
                if (divSeg->PolylineIndex == seg.PolylineIndex)
                    continue;

                crossPoint = GeometryRelation::GetSegmentsCrossPoint(seg.Start, seg.End, divSeg->Start, divSeg->End);

                //jezeli znalezlismy jakies przeciecie to idziemy
                if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
                {
                    //Wybierzmy w ktora strone kolejnego wielomianu bedziemy isc
                    segmentIdxAscending = GetDirectionOnPolylineLeftTurn(seg, *divSeg, segmentIdxAscending);
                    polylineIdx = divSeg->PolylineIndex;
                    segmentIdx = divSeg->SegmentIndex;
                    break;
                }
            }

            if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
                break;
        }

        //Znalezlismy punkt przeciecia z innym wielomianem
        if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
            resultPolyline.emplace_back(crossPoint);

        //Przesuniecia do kolejnego segmentu
        segmentIdx += (segmentIdxAscending ? 1 : -1);
        if (segmentIdx >= m_polylines[polylineIdx].size())//doszliśmy do końca
        {
            resultPolyline.emplace_back(seg.End);
            polylineIdx++;
            segmentIdx = 0;
        } else if (segmentIdx < 0)
        {
            resultPolyline.emplace_back(seg.End);
            polylineIdx--;
            segmentIdx = m_polylines[polylineIdx].size() - 1;
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
            QPoint A = fromPlaneToImage(seg.Start);
            QPoint B = fromPlaneToImage(seg.End);
            p.drawLine(A, B);
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

void PlaneDivision::AddConstraintPolyline(const std::vector<QVector3D> &points, bool flip)
{
    std::vector<QVector2D> planePoints;
    planePoints.reserve(points.size());
    for (const auto &p: points)
        planePoints.emplace_back(QVector2D(p.x(), p.z()));
    AddConstraintPolyline(planePoints, flip);
}

bool PlaneDivision::GetDirectionOnPolylineLeftTurn(const PlaneDivision::PolylineSegment &from,
                                                   const PlaneDivision::PolylineSegment &to, bool fromAscending)
{
    QVector3D fromVec, ascVec;
    //Wektory 3D z zerowym Z
    if (fromAscending)
    {
        fromVec = QVector3D(from.End - from.Start);
        ascVec = QVector3D(to.Start - from.Start);
    }
    else
    {
        fromVec = QVector3D(from.Start - from.End);
        ascVec = QVector3D(to.Start - from.End);
    }

    //Cross w strone rosnaca wyszedl dodatni => skrecamy w lewo do startu
    if (QVector3D::crossProduct(fromVec, ascVec).z() > 0)
        return true;//bedziemy isc w przod kolejnej lamanej
    //Cross w strone rosnaca wyszedl ujemny => skrecamy w prawo do startu, czyli w lewo do Endu
    return false;//bedziemy isc w tyl kolejnej lamanej
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

bool PlaneDivision::GetDirectionOnPolylineRightTurn(const PlaneDivision::PolylineSegment &from,
                                                    const PlaneDivision::PolylineSegment &to, bool fromAscending)
{
    return !GetDirectionOnPolylineLeftTurn(from, to, fromAscending);
}

std::vector<QVector2D>
PlaneDivision::JoinConstraintPolylinesZigzag(std::vector<int> polylineToVisit, bool startAscending, bool firstTurnRight,
                                             int startPolylineIndex, int startPolylineSegment)
{
    int polylineIdx = startPolylineIndex;
    int segmentIdx = startPolylineSegment;
    bool segmentIdxAscending = startAscending;
    bool turnRight = firstTurnRight;

    std::vector<QVector2D> resultPolyline;
    std::set<int> toVisit;

    for (int p: polylineToVisit)
        toVisit.insert(p);

    int turnCounter = 0;
    const int itLimit = 50 * polylineToVisit.size();
    int it = 0;
    do
    {
        try
        {
            auto intersection = GetFirstIntersectFrom(segmentIdxAscending, polylineIdx, segmentIdx, resultPolyline);

            //Oznaczenie jako odwiedzonego wielomianu, z ktroego schodzimy
            toVisit.erase(polylineIdx);

            if (turnRight)
                segmentIdxAscending = GetDirectionOnPolylineRightTurn(*intersection.From, *intersection.To, segmentIdxAscending);
            else
                segmentIdxAscending = GetDirectionOnPolylineLeftTurn(*intersection.From, *intersection.To, segmentIdxAscending);

            polylineIdx = intersection.To->PolylineIndex;
            segmentIdx = intersection.To->SegmentIndex + (segmentIdxAscending ? 1 : -1);
            resultPolyline.emplace_back(intersection.CrossPoint);
        }
        catch (CrossPolylineMissingException &e)//Obsluga braku przeciecia
        {
            //Oznaczenie jako odwiedzonego wielomianu, z ktroego schodzimy
            toVisit.erase(polylineIdx);
            resultPolyline.emplace_back(e.LastPoint);

            if (segmentIdxAscending)//doszliśmy do końca
            {
                polylineIdx = (polylineIdx + 1) % m_polylines.size();
                segmentIdx = 0;
            }
            else
            {
                polylineIdx = polylineIdx == 0 ? m_polylines.size() - 1 : polylineIdx - 1;
                segmentIdx = m_polylines[polylineIdx].size() - 1;
            }
        }

        turnCounter++;
        if (turnCounter > 1)
        {
            turnRight = !turnRight;
            turnCounter = 0;
        }

        it++;
    } while (!toVisit.empty() && it < itLimit);//odwiedzilismy wszystkie okreslone wielomiany

    return resultPolyline;
}

PlaneDivision::PolylineCross
PlaneDivision::GetFirstIntersectFrom(bool direction, int polylineIdx, int segmentIdx,
                                     std::vector<QVector2D> &passedPoints)
{
    int idx = segmentIdx;
    auto poly = m_polylines[polylineIdx];
    PolylineSegment &seg = poly[idx];

    while (idx < poly.size() && idx >= 0)
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

                crossPoint = GeometryRelation::GetSegmentsCrossPoint(seg.Start, seg.End, divSeg->Start, divSeg->End);

                //Jezelie znalezlismy przecięcie to koniec
                if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
                {
                    return PlaneDivision::PolylineCross(&seg, divSeg, &curDiv, crossPoint);
                }
            }
        }

        idx += (direction ? 1 : -1);
    }

    throw CrossPolylineMissingException(direction ? seg.End : seg.Start);
}
