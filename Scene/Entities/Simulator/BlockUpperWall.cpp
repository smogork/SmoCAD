//
// Created by ksm on 10/21/22.
//

#include "BlockUpperWall.h"
#include "Mathematics/PointShapes.h"

BlockUpperWall::BlockUpperWall(QVector3D pos, double widthX, double widthY, int vertexWidthX, int vertexWidthY)
        : IEntity(SIMULATOR_BLOCK_UPPER), m_widthX(widthX), m_widthY(widthY), m_vertexWidthX(vertexWidthX),
          m_vertexWidthY(vertexWidthY)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
    
    m_simulatorMtx.setToIdentity();
    //p_Transform->Rotation = {180, 0, 0};
}

void BlockUpperWall::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    p_Drawing->p_bufferLayout.Push<float>(2);//texCoord
    if (auto sh = Renderer::GetShader(BLOCK_UPPERWALL_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BlockUpperWall::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BlockUpperWall::UniformFunction);
}

void BlockUpperWall::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawTriangles(context->functions(), m_indexCount);
}

void BlockUpperWall::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", m_simulatorMtx * p_Transform->GetModelMatrix());
}

std::vector<float> BlockUpperWall::GenerateGeometryVertices()
{
    std::vector<QVector3D> wall = PointShapes::CreateRect(QVector3D(-m_widthX / 2.0f, 0.0f, -m_widthY / 2.0f),
                                                           m_widthX, m_widthY, m_vertexWidthX, m_vertexWidthY, XZ);
    
    std::vector<float> res;
    
    for (int y = 0; y < m_vertexWidthY; ++y)
    {
        for (int x = 0; x < m_vertexWidthX; ++x)
        {
            res.push_back(wall[y * m_vertexWidthX + x].x());
            res.push_back(wall[y * m_vertexWidthX + x].y());
            res.push_back(wall[y * m_vertexWidthX + x].z());
            res.push_back((float)x / (m_vertexWidthX - 1));
            res.push_back((float)y / (m_vertexWidthY - 1));
        }
    }
    
    return res;
}

std::vector<int> BlockUpperWall::GenerateTopologyIndices()
{
    auto res = PointShapes::RectTriangleIndices(m_vertexWidthX, m_vertexWidthY);
    m_indexCount = res.size();
    return res;
}
