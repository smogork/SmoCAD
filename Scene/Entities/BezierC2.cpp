//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "BezierC2.h"

BezierC2::BezierC2(): IEntity(BEZIERC2_CLASS)
{
    m_bezier.BezierPolyline.DrawingColor = Qt::red;
    DeBoorPolyline.DrawingColor = Qt::blue;

    QObject::connect(DeBoorPolyline.p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BezierC2::OnDeBoorModified);
}

void BezierC2::OnDeBoorModified()
{
    CalculateBezierPoints();
}

void BezierC2::CalculateBezierPoints()
{
    m_bezier.BezierPolyline.p_Collection->Clear();
    bezierPoints.clear();

    if (DeBoorPolyline.p_Collection->Size() < 4)
        return;

    std::vector<QVector3D> result(DeBoorPolyline.p_Collection->Size() * 3 - 4);

    for (int i = 0; i < DeBoorPolyline.p_Collection->Size() - 1; ++i)
    {
        result[3 * i] =  (2 * DeBoorPolyline.p_Collection->operator[](i)->Position +
                DeBoorPolyline.p_Collection->operator[](i+1)->Position) / 3.0f;
        result[3 * i + 1] =  (DeBoorPolyline.p_Collection->operator[](i)->Position +
                              2 * DeBoorPolyline.p_Collection->operator[](i+1)->Position) / 3.0f;
    }

    for (int i = 1; i < DeBoorPolyline.p_Collection->Size() - 1; ++i)
        result[3 * i - 1] = (result[3 * i - 2] + result[3 * i]) / 2.0f;


    for (int i = 2; i < result.size() - 2; ++i)
    {
        auto p = std::make_unique<VirtualPoint>(result[i]);
        m_bezier.BezierPolyline.p_Collection->AddPoint(p->p_CollectionAware);
        bezierPoints.push_back(std::move(p));
    }
}
