//
// Created by ksm on 6/28/22.
//

#include "IntersectionResult.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/IntersectionResultSystem.h"
#include "Scene/Utilities/Utilites.h"

std::shared_ptr<IntersectionResult> IntersectionResult::CreateRegisteredComponent(unsigned int oid, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                                                                                  std::shared_ptr<IntersectionAware> two, bool isCycle)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<IntersectionResultSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->m_paramPoints = points;
            item->m_planeOne = one;
            item->m_planeTwo = two;
            item->m_cycle = isCycle;
            if (item->m_cycle)
                item->m_paramPoints.push_back(item->m_paramPoints.front());
            return item;
        }
    }
    return nullptr;
}

void IntersectionResult::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<IntersectionResultSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

IntersectionResult::IntersectionResult(unsigned int oid): IComponent(oid, INTERSECTION_RESULT)
{

}

IntersectionResult::~IntersectionResult()
{
    UnregisterComponent();
}

QImage IntersectionResult::GetTrimmingTextureOne()
{
    std::vector<QVector2D> args(m_paramPoints.size());

    for (int i = 0; i < m_paramPoints.size(); ++i)
        args[i] = m_paramPoints[i].toVector2D();

    return GetTrimmingTexture(args, m_planeOne);
}

QImage IntersectionResult::GetTrimmingTextureTwo()
{
    std::vector<QVector2D> args(m_paramPoints.size());

    for (int i = 0; i < m_paramPoints.size(); ++i)
        args[i] = {m_paramPoints[i].z(), m_paramPoints[i].w()};

    return GetTrimmingTexture(args, m_planeTwo);
}

QImage IntersectionResult::GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane)
{
    const int size = 512;
    QImage res({size, size}, QImage::Format_Mono);
    res.fill(Qt::color1);

    //zapelnij pixele z krzywej przeciecia
    QPainter painter(&res);
    QBrush brush(Qt::color0, Qt::SolidPattern);
    painter.setBrush(brush);
    for (int i = 1; i < points.size(); ++i)
    {
        bool wrapX = 0, wrapY = 0;
        int x1 = (points[i - 1].x()) * size / plane->UMax;
        int y1 = (points[i - 1].y()) * size / plane->VMax;
        int x2 = (points[i].x()) * size / plane->UMax;
        int y2 = (points[i].y()) * size / plane->VMax;


        if (abs(x2 - x1) > size / 2)
            wrapX = true;
        if (abs(y2 - y1) > size / 2)
            wrapY = true;

        //Jezeli sie zapetlamy, to narysuj dwukrotnie
        if (wrapX)
        {
            if (x1 > size / 2)
            {
                int x2p = std::clamp(x2 + size, 0, size);
                int x1p = std::clamp(x1 - size, 0, size);

                painter.drawLine(x1, y1, x2p, y2);
                painter.drawLine(x1p, y1, x2, y2);
            }
            else
            {
                int x2p = std::clamp(x2 - size, 0, size);
                int x1p = std::clamp(x1 + size, 0, size);

                painter.drawLine(x1, y1, x2p, y2);
                painter.drawLine(x1p, y1, x2, y2);
            }
        }

        if (wrapY)
        {
            if (y1 > size / 2)
            {
                int y1p = std::clamp(y1 - size, 0, size);
                int y2p = std::clamp(y2 + size, 0, size);

                painter.drawLine(x1, y1, x2, y2p);
                painter.drawLine(x1, y1p, x2, y2);
            }
            else
            {
                int y1p = std::clamp(y1 + size, 0, size);
                int y2p = std::clamp(y2 - size, 0, size);

                painter.drawLine(x1, y1, x2, y2p);
                painter.drawLine(x1, y1p, x2, y2);
            }
        }

        if (!wrapX and !wrapY)
            painter.drawLine(x1, y1, x2, y2);
    }

    int x1 = (points[0].x()) * size / plane->UMax;
    int y1 = (points[0].y()) * size / plane->VMax;
    bool test = res.pixelIndex(x1, y1) != 0;

    //zapusc algorytm FloodFill (4spojny) dla pierwszego bialego pixela
    for (int i = 0; i < size * size; ++i)
    {
        int x = i / size;
        int y = i % size;

        if (res.pixelIndex(x, y) != 0)
        {
            FloodFill4({x, y}, 0, res, plane->UWraps, plane->VWraps);
            break;
        }
    }

    return res;
}

void IntersectionResult::FloodFill4(QPoint start, uint color, QImage& image, bool wrapX, bool wrapY)
{

    std::stack<QPoint> s;
    s.push(start);

    while (!s.empty())
    {
        QPoint cur = s.top();
        s.pop();

        if (wrapX)
            cur.setX(wrap(cur.x(), 0, image.width()));

        if (wrapY)
            cur.setY(wrap(cur.y(), 0, image.height()));

        if (cur.x() < 0 || cur.x() >= image.width() ||
            cur.y() < 0 || cur.y() >= image.height())
            continue;

        if (image.pixelIndex(cur) == color)
            continue;

        image.setPixel(cur, color);
        s.push({cur.x() + 1, cur.y()});
        s.push({cur.x(), cur.y() + 1});
        s.push({cur.x() - 1, cur.y()});
        s.push({cur.x(), cur.y() - 1});
    }
}

std::vector<QVector3D> IntersectionResult::GetScenePoints()
{
    std::vector<QVector3D> res(m_paramPoints.size());

    for (int i = 0; i < m_paramPoints.size(); ++i)
        res[i] = m_planeOne->SceneFunction(m_paramPoints[i].toVector2D());

    return res;
}

int IntersectionResult::GetScenePointsSize()
{
    return m_paramPoints.size();
}
