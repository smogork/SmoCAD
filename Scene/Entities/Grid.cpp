//
// Created by ksm on 4/5/22.
//

#include "Grid.h"

std::vector<float> Grid::gridVertices = GenerateGridVertices();
std::vector<int> Grid::gridIndices = GenerateGridIndices();

Grid::Grid() : IEntity(GRID_CLASS)
{
    p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID);

    InitializeDrawing();
}

void Grid::DrawingFunction(QOpenGLContext *context, std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->SetUniform("u_ObjectColor", gridColor);
    shader->Bind();

    Renderer::DrawLines(context->functions(), gridIndices.size());
}

void Grid::InitializeDrawing()
{
    p_Drawing->SetVertexData(gridVertices);
    p_Drawing->SetIndexData(gridIndices);
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Grid::DrawingFunction);
}

std::vector<int> Grid::GenerateGridIndices()
{
    std::vector<int> res(gridVerticesCount * (gridVerticesCount - 1) * 4);

    int offset = 0;

    //poziome linie
    for (int y = 0; y < gridVerticesCount; y++)
    {
        for (int x = 0; x < gridVerticesCount - 1; x++)
        {
            res[offset] = y * gridVerticesCount + x;
            res[offset + 1] = y * gridVerticesCount + x + 1;
            offset += 2;
        }
    }

    for (int y = 0; y < gridVerticesCount - 1; y++)
    {
        for (int x = 0; x < gridVerticesCount; x++)
        {
            res[offset] = y * gridVerticesCount + x;
            res[offset + 1] = (y + 1) * gridVerticesCount + x;
            offset += 2;
        }
    }

    return res;
}

std::vector<float> Grid::GenerateGridVertices()
{
    float startPoint = gridVerticesCount * gridOffset * -0.5;
    std::vector<float> res(gridVerticesCount * gridVerticesCount * 3);

    int offset = 0;
    for (int y = 0; y < gridVerticesCount; y++)
    {
        for (int x = 0; x < gridVerticesCount; x++)
        {
            res[offset] = startPoint + (float) x * gridOffset;
            res[offset + 1] = 0.0f;
            res[offset + 2] = startPoint + (float) y * gridOffset;
            offset += 3;
        }
    }

    return res;
}

