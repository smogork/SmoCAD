//
// Created by ksm on 4/9/22.
//

#include "Polyline.h"

Polyline::Polyline(): IEntity(POLYLINE_CLASS)
{
    p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID);
    p_Collection = TransformCollection::CreateRegisteredComponent(objectID);

    InitializeDrawing();
}

std::vector<float> Polyline::GenerateGeometryVertices()
{
    return std::vector<float>();
}

std::vector<int> Polyline::GenerateTopologyIndices()
{
    return std::vector<int>();
}

void Polyline::InitializeDrawing()
{

}

void Polyline::DrawingFunction(QOpenGLContext *context)
{

}

void Polyline::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{

}


