//
// Created by ksm on 4/22/22.
//

#include "InterpolationC2.h"
#include "Mathematics/Interpolation.h"

InterpolationC2::InterpolationC2(const QString& name): BaseCurve(INTERPOLATIONC2_CLASS)
{
    InitObject(name);
}

std::vector<float> InterpolationC2::GenerateGeometryVertices()
{
    std::vector<float> res (4 * m_bernsteinPoints.size());

    int i = 0;
    for (const std::unique_ptr<InvisiblePoint>& ip : m_bernsteinPoints)
    {
        res[4 * i] = (*ip->p_Transform->Position).x();
        res[4 * i + 1] = (*ip->p_Transform->Position).y();
        res[4 * i + 2] = (*ip->p_Transform->Position).z();
        res[4 * i + 3] = 1.0f;
        i++;
    }

    return res;
}

std::vector<int> InterpolationC2::GenerateTopologyIndices()
{
    if (p_Collection->Size() < 2)
        return {};
    int groups = p_Collection->Size() - 1;
    std::vector<int> res (4 * groups);

    for (int i = 0; i < groups; ++i)
    {
        res[4 * i] = 3 * i;
        res[4 * i + 1] = 3 * i + 1;
        res[4 * i + 2] = 3 * i + 2;
        res[4 * i + 3] = 3 * i + 3;
    }

    return res;
}

int InterpolationC2::GetIndexCount()
{
    if (p_Collection->Size() > 1)
        return 4 * (p_Collection->Size() - 1);
    return 0;
}

void InterpolationC2::OnCollectionModified()
{
    if (p_Collection->Size() == 0)
    {
        m_bernsteinPoints.clear();
        p_Drawing->SetVertexData({});
        p_Drawing->SetIndexData({});
        m_curvePolyline.p_Collection->Clear();
        return;
    }

    std::vector<QVector3D> knots;
    knots.reserve(p_Collection->Size());
    for (const auto& wt : p_Collection->GetPoints())
        if (auto t = wt.lock())
            knots.push_back(t->Position);

    auto bernsteinPoints = Interpolation::C2Interpolation(knots);
    m_bernsteinPoints.clear();
    std::vector<std::shared_ptr<CollectionAware>> elements;
    for (const QVector3D& p : bernsteinPoints)
    {
        auto ip = std::make_unique<InvisiblePoint>(p);
        elements.push_back(ip->p_CollectionAware);
        m_bernsteinPoints.emplace_back(std::move(ip));
    }

    m_curvePolyline.p_Collection->SetPoints(elements);
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

void InterpolationC2::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    OnCollectionModified();
}

void InterpolationC2::SerializingFunction(MG1::Scene &scene)
{
    MG1::InterpolatedC2 i2;
    i2.name = p_SceneElement->Name.value().toStdString();
    CommonSerializeFunction(i2);

    scene.interpolatedC2.push_back(i2);
}

InterpolationC2::InterpolationC2(const MG1::InterpolatedC2 &i2): BaseCurve(INTERPOLATIONC2_CLASS, i2.GetId())
{
    InitObject(i2.name.c_str());
    CommonDeserializeFunction(i2);
}

void InterpolationC2::InitObject(const QString& name)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&InterpolationC2::SerializingFunction);

    PolylineColor = Qt::red;
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &InterpolationC2::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &InterpolationC2::OnSinglePointModified);
    selectedNotifier = p_Selected->Selected.addNotifier([this]()
                                                        {
                                                            if (p_Selected->Selected)
                                                                CurveColor = Selectable::SelectedColor;
                                                            else
                                                                CurveColor = DefaultColor;
                                                        });
}


