//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "BezierC2.h"

#include "Renderer/Options.h"
#include "Scene/Entities/Points/Point.h"

BezierC2::BezierC2(const QString& name): BaseCurve(BEZIERC2_CLASS)
{
    InitializeDrawing();

    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&BezierC2::SerializingFunction);

    selectedNotifier = p_Selected->Selected.addNotifier([this](){
        if (p_Selected->Selected)
            CurveColor = QColor::fromRgbF(1.0f, 0.5f, 0.2f, 1.0f);
        else
            CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);
    });
    deBoorPolylineDrawing = Options::DrawDeBoorPolygon.addNotifier([this]()
       {
           this->m_deBoorPolyline.p_Drawing->Enabled = Options::DrawDeBoorPolygon;
       });

    m_deBoorPolyline.DrawingColor = Qt::blue;
    m_deBoorPolyline.p_Drawing->Enabled = Options::DrawDeBoorPolygon;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BezierC2::OnDeBoorModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &BezierC2::OnSingleBezierPointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &BezierC2::OnDeBoorModified);
}

void BezierC2::OnDeBoorModified()
{
    /*(*m_deBoorPolyline.p_Collection) = (*p_Collection);
    CalculateBezierPoints();*/
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

void BezierC2::CalculateBezierPoints()
{
    //m_bezier.p_Collection->Clear();
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
        //m_bezier.p_Collection->AddPoint(p->p_CollectionAware);
        bezierPoints.push_back(std::move(p));
    }
}

void BezierC2::OnSingleBezierPointModified(QVector3D pos, unsigned int changedOID)
{
    /*int i = 0;
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
    //CalculateBezierPoints();*/
}

std::vector<float> BezierC2::GenerateGeometryVertices()
{
    if (GetIndexCount() == 0)
        return {};

    std::vector<float> res (4 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform>& wp : p_Collection->GetPoints())
    {
        if (auto ip = wp.lock())
        {
            res[4 * i] = (*ip->Position).x();
            res[4 * i + 1] = (*ip->Position).y();
            res[4 * i + 2] = (*ip->Position).z();
            res[4 * i + 3] = 1.0f;
            i++;
        }
    }

    return res;
}

std::vector<int> BezierC2::GenerateTopologyIndices()
{
    std::vector<int> res (GetIndexCount());
    if (GetIndexCount() == 0)
        return {};

    int groups = p_Collection->Size() - 3;
    for (int i = 0; i < groups; ++i)
    {
        res[4 * i] = i;
        res[4 * i + 1] = i + 1;
        res[4 * i + 2] = i + 2;
        res[4 * i + 3] = i + 3;
    }

    return res;
}

int BezierC2::GetIndexCount()
{
    if (p_Collection->Size() < 4)
        return 0;

    return 4 * (p_Collection->Size() - 3);
}

void BezierC2::InitializeDrawing()
{
    if (auto sh = Renderer::GetShader(BEZIERC2_SHADER).lock())
        p_Drawing->AttachShader(sh);
}

void BezierC2::SerializingFunction(MG1::Scene &scene)
{
    MG1::BezierC2 b2;
    b2.name = p_SceneElement->Name.value().toStdString();
    CommonSerializeFunction(b2);

    scene.bezierC2.push_back(b2);
}
