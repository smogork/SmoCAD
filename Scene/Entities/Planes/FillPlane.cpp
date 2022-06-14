//
// Created by ksm on 6/14/22.
//

#include "FillPlane.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"

FillPlane::FillPlane(const QString &name, const std::vector<std::shared_ptr<FillAware>>& edgePlanes)
        : BasePlane(FILL_PLANE_CLASS, false, edgePlanes.size(), 1)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    
    if (auto sh = Renderer::GetShader(FILL_PLANE_SHADER).lock())
        p_Drawing->AttachShader(sh);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&FillPlane::UniformFunction);
    
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &FillPlane::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &FillPlane::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &FillPlane::PointRemovedFromCollection);
    
    selectedNotifier = p_Selected->Selected.addNotifier([this]()
                                                        {
                                                            if (p_Selected->Selected)
                                                                PlaneColor = Selectable::SelectedColor;
                                                            else
                                                                PlaneColor = DefaultColor;
                                                        });
    //MeshColor = Qt::darkRed;
    
    FillCollection(edgePlanes);
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Collection->LockContent();
}

std::vector<float> FillPlane::GenerateGeometryVertices()
{
    std::vector<QVector3D> edgeCoords = p_Collection->GetVectorCoords();
    
    std::vector<QVector3D> coords = PointShapes::CreateFillPlanePoints(edgeCoords);
    
    std::vector<float> res(3 * coords.size());
    
    //jbc to tutaj mozna stworzyc punkty aby obejrzec co wyszlo
    
    int i = 0;
    for (const QVector3D& p:  coords)
    {
        res[3 * i] = p.x();
        res[3 * i + 1] = p.y();
        res[3 * i + 2] = p.z();
        i++;
    }
    
    return res;
}

std::vector<int> FillPlane::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    
    const int indexOffset = 20;
    const int vertexOffset = 14;
    const int vertexSize = vertexOffset * p_UV->U;
    
    for (int i = 0; i < p_UV->U; ++i)
    {
        for (int j = 0; j <= 5; ++j)
            res[indexOffset * i + j] = (vertexOffset * i + j) % vertexSize;
        
        for (int j = 6; j <= 13; ++j)
            res[indexOffset * i + j] = (vertexOffset * i + j - 1) % vertexSize;
        
        res[indexOffset * i + 14] = (vertexOffset * i + 12) % vertexSize;
        res[indexOffset * i + 15] = (vertexOffset * i + 13) % vertexSize;
    
        res[indexOffset * i + 16] = vertexSize; // punkt centralny
    
        res[indexOffset * i + 17] = (vertexOffset * i + 23) % vertexSize;//tu bierzemy juz z znastepnego platka
        res[indexOffset * i + 18] = (vertexOffset * i + 18) % vertexSize;
        res[indexOffset * i + 19] = (vertexOffset * i + 14) % vertexSize;
    }
    
    return res;
}

int FillPlane::GetIndexCount()
{
    return 20 * p_UV->U;
    //return 20;
}

void FillPlane::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    //zaktualizuj siatke
}

void FillPlane::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    //zaktualizuj siatke
}

void FillPlane::PointRemovedFromCollection()
{
    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(GetObjectID());
}

void FillPlane::FillCollection(const std::vector<std::shared_ptr<FillAware>> &edgePlanes)
{
    //[TODO] przerobic to na wiecej niz 3 plaszczyzny
    
    auto one_points = edgePlanes[0]->GetPointsFromEdge(edgePlanes[0]->GetFillEdgeWith(edgePlanes[1], edgePlanes[2]));
    auto two_points = edgePlanes[1]->GetPointsFromEdge(edgePlanes[1]->GetFillEdgeWith(edgePlanes[0], edgePlanes[2]));
    auto three_points = edgePlanes[2]->GetPointsFromEdge(edgePlanes[2]->GetFillEdgeWith(edgePlanes[0], edgePlanes[1]));
    
    //ustaw punkty aby tworzyly jeden ciag
    if (one_points[3]->GetAttachedObjectID() == three_points[3]->GetAttachedObjectID() or
            one_points[3]->GetAttachedObjectID() == three_points[0]->GetAttachedObjectID())
        for (int i = 0; i < 2; ++i)
        {
            std::swap(one_points[i], one_points[3 - i]);
            std::swap(one_points[i + 4], one_points[3 - i + 4]);
        }
        
    if (one_points[3]->GetAttachedObjectID() == two_points[3]->GetAttachedObjectID())//dwojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(two_points[i], two_points[3 - i]);
            std::swap(two_points[i + 4], two_points[3 - i + 4]);
        }
    
    if (two_points[3]->GetAttachedObjectID() == three_points[3]->GetAttachedObjectID())//trojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(three_points[i], three_points[3 - i]);
            std::swap(three_points[i + 4], three_points[3 - i + 4]);
        }
    
    std::vector<std::shared_ptr<CollectionAware>> res;
    for (int j = 0; j < one_points.size(); ++j) res.push_back(one_points[j]);
    for (int j = 0; j < two_points.size(); ++j) res.push_back(two_points[j]);
    for (int j = 0; j < three_points.size(); ++j) res.push_back(three_points[j]);
    
    p_Collection->SetPoints(res);
}

void FillPlane::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    BasePlane::UniformFunction(shader);
    shader->GetRawProgram()->setPatchVertexCount(20);
}
