//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "BezierC2.h"

#include "Renderer/Options.h"

BezierC2::BezierC2(const QString& name): IEntity(BEZIERC2_CLASS)
{
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));

    selectedNotifier = p_Selected->Selected.addNotifier([this](){
        if (p_Selected->Selected)
            m_bezier.CurveColor = QColor::fromRgbF(1.0f, 0.5f, 0.2f, 1.0f);
        else
            m_bezier.CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);
    });
    deBoorPolylineDrawing = Options::DrawDeBoorPolygon.addNotifier([this]()
       {
           this->m_deBoorPolyline.p_Drawing->Enabled = Options::DrawDeBoorPolygon;
       });

    m_bezier.PolylineColor = Qt::red;
    m_deBoorPolyline.DrawingColor = Qt::blue;
    m_deBoorPolyline.p_Drawing->Enabled = Options::DrawDeBoorPolygon;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BezierC2::OnDeBoorModified);
    QObject::connect(m_bezier.p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &BezierC2::OnSingleBezierPointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
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

void BezierC2::OnSingleBezierPointModified(QVector3D pos, unsigned int changedOID)
{
    int i = 0;
    QVector3D oldPos;

    for (auto p : m_bezier.p_Collection->GetPoints())
    {
        if (p.lock()->GetAttachedObjectID() == changedOID)
            break;
        i++;
    }

    auto data = m_bezier.p_Drawing->GetVertex(i, 4);
    oldPos = QVector3D(data[0], data[1], data[2]);

    float transformConst = i % 3 == 0 ? 3.0f / 2.0f : 54.0f / 31.0f;
    auto it = p_Collection->GetPoints().begin();
    for (int bi = 0; bi < (i + 1) / 3 + 1; ++bi)
        ++it;
    if (auto deBoor = it->lock())
    {
        QVector3D dPos = pos - oldPos;
        QVector3D deBoorPosition = deBoor->Position;
        deBoor->Position = deBoorPosition + transformConst * dPos;
    }
    //CalculateBezierPoints();
}
