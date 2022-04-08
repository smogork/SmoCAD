//
// Created by ksm on 4/6/22.
//

#include "Cursor.h"

std::vector<float> Cursor::vertices = GenerateGeometryVertices();
std::vector<int> Cursor::indices = GenerateTopologyIndices();

std::vector<float> Cursor::GenerateGeometryVertices()
{
    return {
            0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
}

std::vector<int> Cursor::GenerateTopologyIndices()
{
    return {
            0, 1,
            0, 2,
            0, 3,
    };
}

void Cursor::InitializeDrawing()
{
    p_Drawing->SetVertexData(vertices);
    p_Drawing->SetIndexData(indices);
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    p_Drawing->p_bufferLayout.Push<float>(3);//color
    if (auto sh = Renderer::GetShader(SHADERS::CURSOR_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Cursor::DrawingFunction);
}

void Cursor::DrawingFunction(QOpenGLContext *context, std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->Bind();

    Renderer::DrawLines(context->functions(), indices.size());
}

Cursor::Cursor(QVector3D position): IEntity(ENTITY_CLASS::CURSOR_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, position);
    p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID);

    InitializeDrawing();
}