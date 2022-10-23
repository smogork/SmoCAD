//
// Created by ksm on 10/21/22.
//

#include "BlockLowerWall.h"
#include "Mathematics/PointShapes.h"

BlockLowerWall::BlockLowerWall(QVector3D pos, std::shared_ptr<Transform> simulatorTransform, double widthX, double widthY)
        : IEntity(SIMULATOR_BLOCK_LOWER), m_widthX(widthX), m_widthY(widthY), m_simulatorTransform(simulatorTransform)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
    
    p_Transform->Scale = QVector3D(m_widthX, 1, m_widthY);
    p_Transform->Rotation = QVector3D(180, 0, 0);
}

void BlockLowerWall::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    QMatrix4x4 modelMtx = p_Transform->GetModelMatrix();
    if (auto trans = m_simulatorTransform.lock())
        modelMtx = trans->GetModelMatrix() * modelMtx;
    shader->SetUniform("u_MVP.Model", modelMtx);
}

void BlockLowerWall::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawTriangles(context->functions(), 6);
}

void BlockLowerWall::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    p_Drawing->p_bufferLayout.Push<float>(2);//texCoord
    if (auto sh = Renderer::GetShader(BLOCK_LOWERWALL_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BlockLowerWall::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BlockLowerWall::UniformFunction);
}

std::vector<float> BlockLowerWall::GenerateGeometryVertices()
{
    auto points = PointShapes::CreateSingleRect({}, 1, 1, XZ);
    
    return
            {
                    points[0].x(), points[0].y(), points[0].z(), 0.0f, 0.0f,
                    points[1].x(), points[1].y(), points[1].z(), 1.0f, 0.0f,
                    points[2].x(), points[2].y(), points[2].z(), 0.0f, 1.0f,
                    points[3].x(), points[3].y(), points[3].z(), 1.0f, 1.0f,
            };
}

std::vector<int> BlockLowerWall::GenerateTopologyIndices()
{
    //return {0, 2, 1, 1, 2, 3};//counter clockwise?
    return {0, 1, 2, 1, 3, 2};//clockwise?
}
