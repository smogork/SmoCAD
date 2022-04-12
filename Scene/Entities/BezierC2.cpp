//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "BezierC2.h"

BezierC2::BezierC2(): IEntity(BEZIERC2_CLASS)
{
    p_Collection = TransformCollection::CreateRegisteredComponent(objectID);

    m_bezier.PolylineColor = Qt::red;
    m_deBoorPolyline.DrawingColor = Qt::blue;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BezierC2::OnDeBoorModified);
}

void BezierC2::OnDeBoorModified()
{
    (*m_deBoorPolyline.p_Collection) = (*p_Collection);
    CalculateBezierPoints();
}

void BezierC2::CalculateBezierPoints()
{
    m_bezier.p_Collection->Clear();
    bezierPoints.clear();

    if (p_Collection->Size() < 4)
        return;

    std::vector<QVector3D> result(p_Collection->Size() * 3 - 4);
    auto it = p_Collection->GetPoints().begin();
    auto end = p_Collection->GetPoints().end();
    std::shared_ptr P1 = it->lock();
    ++it;
    for (int i = 0; i < p_Collection->Size() - 1 && it != end; ++i)
    {
        std::shared_ptr P2 = it->lock();
        if (!P1 || !P2)
        {
            P1 = P2;
            ++it;
            continue;
        }

        result[3 * i] =  (2 * P1->Position + P2->Position) / 3.0f;
        result[3 * i + 1] =  (P1->Position + 2 * P2->Position) / 3.0f;

        P1 = P2;
        ++it;
    }

    for (int i = 1; i < p_Collection->Size() - 1; ++i)
        result[3 * i - 1] = (result[3 * i - 2] + result[3 * i]) / 2.0f;


    for (int i = 2; i < result.size() - 2; ++i)
    {
        auto p = std::make_unique<VirtualPoint>(result[i]);
        m_bezier.p_Collection->AddPoint(p->p_CollectionAware);
        bezierPoints.push_back(std::move(p));
    }
}
