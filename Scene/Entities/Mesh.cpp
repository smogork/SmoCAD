//
// Created by ksm on 5/15/22.
//

#include "Mesh.h"
#include "Scene/Utilities/Utilites.h"

Mesh::Mesh(): IEntity(MESH_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(GetObjectID()));

    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &Mesh::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &Mesh::OnSinglePointModified);
}

void Mesh::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

void Mesh::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

std::vector<float> Mesh::GenerateGeometryVertices()
{
    //zabezpiecznenie przed nieprostokątnymi danymi w kolekcji
    if (p_Collection->SecondDimension == 0 || p_Collection->Size() % p_Collection->SecondDimension)
        return std::vector<float>();

    std::vector<float> res;
    res.reserve(p_Collection->Size() * 3);

    for (auto elw : p_Collection->GetPoints())
        if (auto el = elw.lock())
        {
            res.push_back((*el->Position).x());
            res.push_back((*el->Position).y());
            res.push_back((*el->Position).z());
        }

    return res;
}

std::vector<int> Mesh::GenerateTopologyIndices()
{
    //zabezpiecznenie przed nieprostokątnymi danymi w kolekcji
    if (p_Collection->SecondDimension == 0 || p_Collection->Size() % p_Collection->SecondDimension)
        return std::vector<int>();

    std::vector<int> res;
    int height = p_Collection->Size() / p_Collection->SecondDimension;
    int width = p_Collection->SecondDimension;
    res.reserve(GetIndexCount());

    //pionowe linie
    for (int n = 0; n < width; ++n)
    {
        for (int i = 1; i < height; ++i)
        {
            res.push_back((i - 1) * width + n);
            res.push_back(i * width + n);
        }
    }

    //poziome linie
    for (int n = 1; n < width; ++n)
    {
        for (int i = 0; i < height; ++i)
        {
            res.push_back(i * width + n);
            res.push_back(i * width + n - 1);
        }
    }

    if (IsPipe)
    {
        for (int i = 0; i < height; ++i)
        {
            res.push_back(i * width + width - 1);
            res.push_back(i * width);
        }
    }

    return res;
}

void Mesh::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Mesh::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&Mesh::UniformFunction);
    //p_Drawing->Enabled = false;
}

void Mesh::DrawingFunction(QOpenGLContext *context)
{
    if (p_Collection->Size())
        Renderer::DrawLines(context->functions(), GetIndexCount());
}

void Mesh::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

int Mesh::GetIndexCount()
{
    if (p_Collection->Size() == 0)
        return 0;

    int height = p_Collection->Size() / p_Collection->SecondDimension;
    int width = p_Collection->SecondDimension;
    int res = 2 * p_Collection->Size() - width - height;
    if (IsPipe)
        res += height;

    return 2 * res;
}

