//
// Created by ksm on 5/30/22.
//

#include "PlaneC2.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"
#include "ThirdParty/Scene-Serializer/cpp/Serializer/Serializer/Scene/SerializerException.h"
#include "Scene/Systems/CollectionAwareSystem.h"

void PlaneC2::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC2::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC2::PointRemovedFromCollection()
{
    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(GetObjectID());
}

PlaneC2::PlaneC2(const QString &name, bool isPipe, int countU, int countV): BasePlane(PLANEC2_CLASS, isPipe, countU, countV)
{
    InitObject(name, isPipe, countU, countV);
}

std::vector<float> PlaneC2::GenerateGeometryVertices()
{
    std::vector<float> res (3 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform>& pw : p_Collection->GetPoints())
        if (auto p = pw.lock())
        {
            res[3 * i] = (*p->Position).x();
            res[3 * i + 1] = (*p->Position).y();
            res[3 * i + 2] = (*p->Position).z();
            i++;
        }

    return res;
}

std::vector<int> PlaneC2::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    int res_idx = 0;

    int index_width =  p_UV->U + (PATCH_SIZE - 1);
    if (p_UV->UWraps)
        index_width -= PATCH_SIZE - 1;
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
            for (int i = 0; i < PATCH_SIZE; ++i)//height
                for (int j = 0; j < PATCH_SIZE; ++j)//width
                {
                    int wIdx = w  + j;
                    int hIdx = h  + i;
                    res[res_idx++] = hIdx * index_width + (wIdx % index_width);
                }

    return res;
}

int PlaneC2::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

std::vector<std::shared_ptr<Point>>
PlaneC2::CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width,
                              float height)
{
    std::vector<std::shared_ptr<Point>> res;

    if (auto scene = SceneECS::Instance().lock())
    {
        std::vector<QVector3D> positions;
        if (isPipe)
            positions = PointShapes::CreateTube(startPos - QVector3D(0, 0, height/ V), width, height * (1 + 2.0f / V), U , V + 3);
        else
            positions = PointShapes::CreateRect(startPos - QVector3D(width / U, 0, height/ V), width * (1 + 2.0f / U), height * (1 + 2.0f / V), U + 3, V + 3);

        for (int i = 0; i < positions.size(); ++i)
        {
            QString pName = QString("P_%0_%1").arg(name).arg(i);
            auto p = std::make_shared<Point>(pName, positions[i]);
            res.emplace_back(p);
            scene->AddObjectExplicitPosition(p);
        }
    }

    return res;
}

void PlaneC2::SerializingFunction(MG1::Scene &scene)
{
    MG1::BezierSurfaceC2 p2;
    p2.SetId(GetObjectID());
    p2.name = p_SceneElement->Name.value().toStdString();
    p2.uWrapped = p_UV->UWraps;
    p2.vWrapped = p_UV->VWraps;
    p2.size.x = p_UV->U;
    p2.size.y = p_UV->V;

    auto indices = GenerateTopologyIndices();
    std::vector<MG1::PointRef> points;
    for (const auto& wp :  p_Collection->GetPoints())
        if (auto p = wp.lock())
            points.emplace_back(MG1::PointRef(p->GetAttachedObjectID()));

    //[TODO] zrobic wspolna serializacje patchy na podstawie tablicy indeksow oraz punktow
    for (int i = 0; i < GetPatchCount(); ++i)
    {
        MG1::BezierPatchC2 patch;

        patch.name = p_SceneElement->Name.value().toStdString();//[TODO] dodac inne nazwy dla patchy
        patch.samples.x = p_UV->UDensity;
        patch.samples.y = p_UV->VDensity;

        for (int j = 0; j < PATCH_SIZE * PATCH_SIZE; ++j)
        {
            patch.controlPoints.emplace_back(points[indices[i * (PATCH_SIZE * PATCH_SIZE) + j]]);
        }

        p2.patches.emplace_back(patch);
    }

    scene.surfacesC2.push_back(p2);
}

PlaneC2::PlaneC2(const MG1::BezierSurfaceC2 &p2): BasePlane(PLANEC2_CLASS, p2.GetId(), p2.uWrapped, p2.size.x, p2.size.y)
{
    InitObject(p2.name.c_str(), p2.uWrapped, p2.size.x, p2.size.y);

    p_UV->LockEditUV(false);
    p_Collection->LockContent(false);
    p_UV->VWraps = p2.vWrapped;

    if (p2.patches.size() == 0)
        throw MG1::SerializerException("Error while deserializing Surface without patches");

    p_UV->UDensity = p2.patches[0].samples.x;
    p_UV->VDensity = p2.patches[0].samples.y;
    p_UV->LockEditUV();


    auto indices = GenerateTopologyIndices();
    std::vector<std::shared_ptr<CollectionAware>> points(GetVertexCount(p_UV->UWraps));

    //[TODO] zrobic wspolna deserializacje patchy na podstawie tablicy indeksow oraz punktow
    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < GetPatchCount(); ++i)
        {
            MG1::BezierPatchC2 patch = p2.patches[i];

            for (int j = 0; j < PATCH_SIZE * PATCH_SIZE; ++j)
            {
                int idx = indices[i * (PATCH_SIZE * PATCH_SIZE) + j];//index punktu w kolekcji punktow
                if (!points[idx])//jeżeli jeszcze punkt nie został ustalony
                {
                    MG1::PointRef ref = patch.controlPoints[j];

                    if (auto el = scene->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(ref.GetId()).lock())
                        points[idx] = el;//przypisanie referencji do odpowiedniego miejsca w powierzchni
                    else
                        throw MG1::SerializerException("Unknown point during deserialization of SurfaceC0");
                }
            }
        }

        p_Collection->SetPoints(points);
        p_Collection->LockContent();
    }
}

int PlaneC2::GetVertexCount(bool isPipe)
{
    if (isPipe)
        return  p_UV->U * (p_UV->V + 3);
    return (p_UV->U + 3) * (p_UV->V + 3);
}

void PlaneC2::InitObject(const QString &name, bool isPipe, int countU, int countV)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&PlaneC2::SerializingFunction);

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension =
            isPipe ? countU + 3 : countU;
    if (auto sh = Renderer::GetShader(PLANEC2_SHADER).lock())
        p_Drawing->AttachShader(sh);

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlaneC2::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlaneC2::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &PlaneC2::PointRemovedFromCollection);

    selectedNotifier = p_Selected->Selected.addNotifier([this]() {
        if (p_Selected->Selected)
            PlaneColor = Selectable::SelectedColor;
        else
            PlaneColor = DefaultColor;
    });
    MeshColor = Qt::darkYellow;
}
