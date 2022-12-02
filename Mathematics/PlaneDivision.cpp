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

std::list<PlaneDivision::ConstraintSegment>& PlaneDivision::GetDivision(int col, int row)
{
    return m_division[row * m_size + col];
}

void PlaneDivision::AddConstraintPolyline(const std::vector<QVector2D> &points, bool flip)
{
    if (flip)
    {
        std::vector<QVector2D> temp;
        temp.insert(temp.end(), points.rbegin(), points.rend());
        m_polylines.emplace_back(temp);
    }
    else
        m_polylines.emplace_back(points);
}

std::vector<QVector2D> &PlaneDivision::GetConstraintPolyline(int index)
{
    return m_polylines[index];
}

void PlaneDivision::CreateDivision(QVector4D planeSize, int divisionCount)
{
    m_size = divisionCount;
    m_division = new std::list<ConstraintSegment>[m_size * m_size];

    float dx = (planeSize.z() - planeSize.x()) / m_size;
    float dy = (planeSize.w() - planeSize.y()) / m_size;

    for (int y = 0; y < m_size; ++y)
        for (int x = 0; x < m_size; ++x)
        {
            QVector4D rect = {planeSize.x() + dx * x, planeSize.y() + dy * y, 0, 0};
            rect.setZ(rect.x() + dx);
            rect.setW(rect.y() + dy);

            for (int j = 0; j < m_polylines.size(); ++j)
            {
                const auto &polyline = m_polylines[j];
                for (int i = 1; i < polyline.size(); ++i)
                {
                    if (GeometryRelation::IsSegmentInsideRect(polyline[i - 1], polyline[i], rect))
                        GetDivision(x, y).emplace_back(ConstraintSegment(polyline[i - 1], polyline[i], j, i - 1));
                }
            }
        }

    m_divisionCreated = true;
    DebugImageOfPlane(planeSize);
}

std::vector<QVector2D> PlaneDivision::JoinConstraintPolylinesTogether(int startPolylineIndex)
{
    return std::vector<QVector2D>();
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
        for (int i = 1; i < polyline.size(); ++i)
        {
            QPoint A = fromPlaneToImage(polyline[i - 1]);
            QPoint B = fromPlaneToImage(polyline[i]);
            p.drawLine(A, B);
        }
        polylineColor = (Qt::GlobalColor)(polylineColor + 1);
    }

    //narysuj poczatki i konce lamanych
    for (const auto &polyline : m_polylines)
    {
        QPoint S = fromPlaneToImage(polyline.front());
        QPoint E = fromPlaneToImage(polyline.back());
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
    AddConstraintPolyline(planePoints);
}
