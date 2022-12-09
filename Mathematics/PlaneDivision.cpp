//
// Created by ksm on 12/2/22.
//

#include <QImage>
#include <QPainter>
#include "PlaneDivision.h"
#include "GeometryRelation.h"

PlaneDivision::PlaneDivision()
{

}

PlaneDivision::~PlaneDivision()
{
    if (m_divisionCreated)
        delete [] m_division;
}

PlaneDivision::Division& PlaneDivision::GetDivision(int col, int row)
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
    }
    else
    {
        for (int i = 1; i < points.size(); ++i)
            insidePoly.emplace_back(PolylineSegment(points[i - 1], points[i], m_polylines.size(), counter++));
    }

    m_polylines.emplace_back(insidePoly);
}

/*std::vector<QVector2D> &PlaneDivision::GetConstraintPolyline(int index)
{
    return m_polylines[index];
}*/

void PlaneDivision::CreateDivision(QVector4D planeSize, int divisionCount)
{
    m_size = divisionCount;
    m_division = new Division[m_size * m_size];

    float dx = (planeSize.z() - planeSize.x()) / m_size;
    float dy = (planeSize.w() - planeSize.y()) / m_size;

    for (int y = 0; y < m_size; ++y)
        for (int x = 0; x < m_size; ++x)
        {
            Division& curDiv = GetDivision(x, y);
            curDiv.Start = {planeSize.x() + dx * x, planeSize.y() + dy * y};
            curDiv.End = curDiv.Start + QVector2D(dx, dy);
            auto rect = curDiv.GetRect();

            for (int j = 0; j < m_polylines.size(); ++j)
            {
                for (PolylineSegment &seg : m_polylines[j])
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
    DebugImageOfPlane(planeSize);
}

std::vector<QVector2D> PlaneDivision::JoinConstraintPolylinesTogether(int startPolylineIndex)
{
    std::vector<QVector2D> resultPolyline;

    int polylineIdx = startPolylineIndex;
    int segmentIdx = 0;
    bool segmentIdxAscending = true;
    do
    {
        PolylineSegment& seg = m_polylines[polylineIdx][segmentIdx];
        resultPolyline.emplace_back(segmentIdxAscending ? seg.Start : seg.End);

        //Sprawdzmy czy rozwazany segment nie przecina sie z innymi
        QVector2D crossPoint = {NAN, NAN};
        for (const auto divIdx : seg.InsideDivisions)
        {
            Division& curDiv = GetDivision(divIdx.first, divIdx.second);
            for (PolylineSegment* divSeg : curDiv.SegmentsInside)
            {
                if (divSeg->PolylineIndex == seg.PolylineIndex)
                    continue;

                crossPoint = GeometryRelation::GetSegmentsCrossPoint(seg.Start, seg.End, divSeg->Start, divSeg->End);

                //jezeli znalezlismy jakies przeciecie to idziemy
                if (!std::isnan(crossPoint.x()) && !std::isnan(crossPoint.y()))
                {
                    //Wybierzmy w ktora strone kolejnego wielomianu bedziemy isc
                    segmentIdxAscending = GetDirectionOnPolylineOuterChange(seg, *divSeg);
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
        }
        else if (segmentIdx < 0)
        {
            resultPolyline.emplace_back(seg.End);
            polylineIdx--;
            segmentIdx = m_polylines[polylineIdx].size() - 1;
        }


    } while ((segmentIdx != 0 || polylineIdx != startPolylineIndex)
    && polylineIdx < m_polylines.size() && polylineIdx >= 0);//Wróciliśmy do początku

    return resultPolyline;
}

void PlaneDivision::DebugImageOfPlane(QVector4D planeSize)
{
    if (!m_divisionCreated)
        return;
    //16 pixeli na klatke podzialu
    QImage plane(QSize(16 * m_size, 16 * m_size), QImage::Format_ARGB32);

    auto fromPlaneToImage = [planeSize, &plane](QVector2D planePoint) -> QPoint
    {
        float widthX = planeSize.z() - planeSize.x();
        float widthY = planeSize.w() - planeSize.y();

        return QPoint(
                (planePoint.y() - planeSize.x()) / widthX * plane.width(),
                (1.0f - ((planePoint.x() - planeSize.y()) / widthY)) * plane.height()
        );
    };

    QPainter p(&plane);
    p.fillRect(QRect(0, 0, plane.width(), plane.height()), Qt::black);
    p.setPen(Qt::darkGray);

    //Narysuj kratki podzialu
    float dx = (planeSize.z() - planeSize.x()) / m_size;
    float dy = (planeSize.w() - planeSize.y()) / m_size;
    for (int y = 0; y < m_size; ++y)
        for (int x = 0; x < m_size; ++x)
        {
            QVector2D leftLower = {planeSize.x() + dx * x, planeSize.y() + dy * y};
            QVector2D rightUpper = leftLower + QVector2D(dx, dy);
            QRect r(fromPlaneToImage(leftLower), fromPlaneToImage(rightUpper));
            p.drawRect(r);
        }

    //Narysuj linie lamanych
    Qt::GlobalColor polylineColor = Qt::white;
    for (const auto &polyline : m_polylines)
    {
        p.setPen(polylineColor);
        for (const PolylineSegment &seg : polyline)
        {
            QPoint A = fromPlaneToImage(seg.Start);
            QPoint B = fromPlaneToImage(seg.End);
            p.drawLine(A, B);
        }
        polylineColor = (Qt::GlobalColor)(polylineColor + 1);
    }

    //narysuj poczatki i konce lamanych
    for (const auto &polyline : m_polylines)
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
    for (const auto& p : points)
        planePoints.emplace_back(QVector2D(p.x(), p.z()));
    AddConstraintPolyline(planePoints, flip);
}

bool PlaneDivision::GetDirectionOnPolylineOuterChange(const PlaneDivision::PolylineSegment &from,
                                                      const PlaneDivision::PolylineSegment &to)
{
    //Wektory 3D z zerowym Z
    QVector3D fromVec(from.End - from.Start);
    QVector3D ascVec(to.Start - from.Start);
    //QVector3D descVec(to.End - from.Start);

    //Cross w strone rosnaca wyszedl dodatni => skrecamy w lewo do startu
    if (QVector3D::crossProduct(fromVec, ascVec).z() > 0)
        return true;//bedziemy isc w przod kolejnej lamanej
    //Cross w strone rosnaca wyszedl ujemny => skrecamy w prawo do startu, czyli w lewo do Endu
    return false;//bedziemy isc w tyl kolejnej lamanej
}
