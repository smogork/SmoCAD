//
// Created by ksm on 5/14/22.
//

#include "PlaneC0.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"

void PlaneC0::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC0::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC0::PointRemovedFromCollection()
{

    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(GetObjectID());
}

std::vector<float> PlaneC0::GenerateGeometryVertices()
{
    std::vector<float> res(3 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform> &pw: p_Collection->GetPoints())
        if (auto p = pw.lock())
        {
            res[3 * i] = (*p->Position).x();
            res[3 * i + 1] = (*p->Position).y();
            res[3 * i + 2] = (*p->Position).z();
            i++;
        }

    return res;
}

std::vector<int> PlaneC0::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    int res_idx = 0;

    int index_width = (PATCH_SIZE - 1) * p_UV->U + 1;
    if (p_UV->UWraps)
        index_width--;
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
            for (int i = 0; i < PATCH_SIZE; ++i)//height
                for (int j = 0; j < PATCH_SIZE; ++j)//width
                {
                    int wIdx = w * (PATCH_SIZE - 1) + j;
                    int hIdx = h * (PATCH_SIZE - 1) + i;
                    res[res_idx++] = hIdx * index_width + (wIdx % index_width);
                }

    return res;
}

int PlaneC0::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

PlaneC0::PlaneC0(const QString &name, bool isPipe, int countU, int countV) :
        BasePlane(PLANEC0_CLASS, isPipe, countU, countV)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&PlaneC0::SerializingFunction);

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension =
            isPipe ? (PATCH_SIZE - 1) * countU : (PATCH_SIZE - 1) * countU + 1;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlaneC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlaneC0::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &PlaneC0::PointRemovedFromCollection);

    selectedNotifier = p_Selected->Selected.addNotifier([this]()
                                                        {
                                                            if (p_Selected->Selected)
                                                                PlaneColor = Selectable::SelectedColor;
                                                            else
                                                                PlaneColor = DefaultColor;
                                                        });
    MeshColor = Qt::darkGreen;
}

std::vector<std::shared_ptr<Point>>
PlaneC0::CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width,
                              float height)
{
    std::vector<std::shared_ptr<Point>> res;

    if (auto scene = SceneECS::Instance().lock())
    {
        std::vector<QVector3D> positions;
        if (isPipe)
            positions = PointShapes::CreateTube(startPos, width, height, (PATCH_SIZE - 1) * U, (PATCH_SIZE - 1) * V + 1);
        else
            positions = PointShapes::CreateRect(startPos, width, height, (PATCH_SIZE - 1) * U + 1, (PATCH_SIZE - 1) * V + 1);

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

void PlaneC0::SerializingFunction(MG1::Scene &scene)
{
    MG1::BezierSurfaceC0 p0;
    p0.SetId(GetObjectID());
    p0.name = p_SceneElement->Name.value().toStdString();
    p0.uWrapped = p_UV->UWraps;
    p0.vWrapped = p_UV->VWraps;
    p0.size.x = p_UV->U;
    p0.size.y = p_UV->V;

    auto indices = GenerateTopologyIndices();
    std::vector<MG1::PointRef> points;
    for (const auto& wp :  p_Collection->GetPoints())
        if (auto p = wp.lock())
            points.emplace_back(MG1::PointRef(p->GetAttachedObjectID()));

    for (int i = 0; i < GetPatchCount(); ++i)
    {
        MG1::BezierPatchC0 patch;

        patch.name = p_SceneElement->Name.value().toStdString();//[TODO] dodac inne nazwy dla patchy
        patch.samples.x = p_UV->UDensity;
        patch.samples.y = p_UV->VDensity;

        for (int j = 0; j < PATCH_SIZE * PATCH_SIZE; ++j)
        {
            patch.controlPoints.emplace_back(points[indices[i * (PATCH_SIZE * PATCH_SIZE) + j]]);
        }

        p0.patches.emplace_back(patch);
    }

    scene.surfacesC0.push_back(p0);
}


