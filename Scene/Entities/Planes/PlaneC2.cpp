//
// Created by ksm on 5/30/22.
//

#include "PlaneC2.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"

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
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension =
            isPipe ? countU + 3 : countU;

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
                    int wIdx = w * (PATCH_SIZE - 1) + j;
                    int hIdx = h * (PATCH_SIZE - 1) + i;
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
            positions = PointShapes::CreateTube(startPos, width, height, U , V + 3);
        else
            positions = PointShapes::CreateRect(startPos - QVector3D(width / U, 0, width / U), width, height, U + 3, V + 3);

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
