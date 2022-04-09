//
// Created by ksm on 4/3/22.
//

#include "Cube.h"

std::vector<float> Cube::vertices = GenerateGeometryVertices();
std::vector<int> Cube::indices = GenerateTopologyIndices();

std::vector<float> Cube::GenerateGeometryVertices()
{
    return {
            +0.5f, +0.5f, +0.5f,  // front top right 0
            +0.5f, -0.5f, +0.5f,  // front bottom right 1
            -0.5f, -0.5f, +0.5f,  // front bottom left 2
            -0.5f, +0.5f, +0.5f,   // front top left 3
            +0.5f, +0.5f, -0.5f,  // back top right 4
            +0.5f, -0.5f, -0.5f,  // back bottom right 5
            -0.5f, -0.5f, -0.5f,  // back bottom left 6
            -0.5f, +0.5f, -0.5f  //back top left 7
    };
}

std::vector<int> Cube::GenerateTopologyIndices()
{
    return {
            0, 1,
            1, 2,
            2, 3,
            0, 3,
            4, 5,
            5, 6,
            6, 7,
            4, 7,
            3, 7,
            2, 6,
            1, 5,
            0, 4,
    };
}


Cube::Cube(QVector3D position): IEntity(CUBE_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, position);
    p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID);
    InitializeDrawing();
}

void Cube::InitializeDrawing()
{
    p_Drawing->SetVertexData(vertices);
    p_Drawing->SetIndexData(indices);
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Cube::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&Cube::UniformFunction);
}

void Cube::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLines(context->functions(), indices.size());
}

void Cube::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
}

