//
// Created by ksm on 6/14/22.
//

#include "FillPlane.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"
#include "Scene/Systems/FillAwareSystem.h"

FillPlane::FillPlane(const QString &name, const FillLoop& planeLoop)
        : BasePlane(FILL_PLANE_CLASS, false, planeLoop.loop.size(), 1)
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
    
    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                if (p_Selected->Selected)
                    PlaneColor = Selectable::SelectedColor;
                else
                    PlaneColor = DefaultColor;
            });
    m_mesh.p_Drawing->Enabled = false;
    
    p_Collection->SetPoints(planeLoop.GetNormalizedLoopPoints());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    
    p_Collection->LockContent();
    p_UV->LockEditUV();
    
    m_gmesh.DrawingColor = Qt::red;
    //[TODO] dodac przelaczanie siatki w opcjach
}

std::vector<float> FillPlane::GenerateGeometryVertices()
{
    std::vector<QVector3D> edgeCoords = p_Collection->GetVectorCoords();
    
    if (edgeCoords.size() < 24)
        return {};
    
    std::vector<QVector3D> coords = PointShapes::CreateFillPlanePoints(edgeCoords);
    
    std::vector<float> res(3 * coords.size());
    
    //jbc to tutaj mozna stworzyc punkty aby obejrzec co wyszlo
    m_gmesh.SetGregoryPoints(coords);
    
    int i = 0;
    for (const QVector3D &p: coords)
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
    
    for (int i = 0; i < GetIndexCount(); ++i)
        res[i] = i;
    
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

void FillPlane::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    BasePlane::UniformFunction(shader);
    shader->GetRawProgram()->setPatchVertexCount(20);
}
