//
// Created by ksm on 6/14/22.
//

#include "GregoryMesh.h"
#include "Scene/Utilities/Utilites.h"

GregoryMesh::GregoryMesh(): IEntity(GREGORY_MESH)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
}

void GregoryMesh::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData({});
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&GregoryMesh::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&GregoryMesh::UniformFunction);
    //p_Drawing->Enabled = false;
}

void GregoryMesh::DrawingFunction(QOpenGLContext *context)
{
    if (GetIndexCount())
        Renderer::DrawLines(context->functions(), GetIndexCount());
}

void GregoryMesh::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

void GregoryMesh::SetGregoryPoints(const std::vector<QVector3D> &points)
{
    m_indexCount = points.size() * 2;
    p_Drawing->SetIndexData(GenerateTopologyIndices(points.size() / 20));
    
    std::vector<float> vertices(points.size() * 3);
    
    int i = 0;
    for (const QVector3D& p:  points)
    {
        vertices[3 * i] = p.x();
        vertices[3 * i + 1] = p.y();
        vertices[3 * i + 2] = p.z();
        i++;
    }
    
    p_Drawing->SetVertexData(vertices);
}

std::vector<int> GregoryMesh::GenerateTopologyIndices(int patchCount)
{
    std::vector<int> res;
    
    for (int i = 0; i < patchCount; ++i)
    {
        res.push_back(20 * i + 0);
        res.push_back(20 * i + 1);
        res.push_back(20 * i + 2);
        res.push_back(20 * i + 1);
        res.push_back(20 * i + 3);
        res.push_back(20 * i + 2);
        res.push_back(20 * i + 9);
        res.push_back(20 * i + 3);
        res.push_back(20 * i + 9);
        res.push_back(20 * i + 15);
        res.push_back(20 * i + 15);
        res.push_back(20 * i + 19);
        res.push_back(20 * i + 19);
        res.push_back(20 * i + 18);
        res.push_back(20 * i + 18);
        res.push_back(20 * i + 17);
        res.push_back(20 * i + 17);
        res.push_back(20 * i + 16);
        res.push_back(20 * i + 16);
        res.push_back(20 * i + 10);
        res.push_back(20 * i + 10);
        res.push_back(20 * i + 4);
        res.push_back(20 * i + 4);
        res.push_back(20 * i + 0);
        res.push_back(20 * i + 4);
        res.push_back(20 * i + 5);
        res.push_back(20 * i + 1);
        res.push_back(20 * i + 6);
        res.push_back(20 * i + 2);
        res.push_back(20 * i + 7);
        res.push_back(20 * i + 8);
        res.push_back(20 * i + 9);
        res.push_back(20 * i + 14);
        res.push_back(20 * i + 15);
        res.push_back(20 * i + 13);
        res.push_back(20 * i + 18);
        res.push_back(20 * i + 12);
        res.push_back(20 * i + 17);
        res.push_back(20 * i + 10);
        res.push_back(20 * i + 11);
    }
    
    return res;
}

int GregoryMesh::GetIndexCount()
{
    return m_indexCount;
}
