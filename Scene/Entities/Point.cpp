//
// Created by ksm on 4/3/22.
//

#include "Point.h"

Point::Point(QVector3D pos): IEntity(POINT_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID);
    p_Drawing = Drawing::CreateRegisteredComponent(objectID);

    p_Transform->Position = pos;
    InitializeDrawing();
}

Point::~Point()
{
}

void Point::InitializeDrawing()
{
    p_Drawing->p_vertexArrayData = {0.0f, 0.0f, 0.0f};
    p_Drawing->p_indexArrayData = {0};
    p_Drawing->p_bufferLayout.Push<float>(3);//position

    //https://stackoverflow.com/a/7582576/18323924
    p_Drawing->p_renderingFunction = std::bind(&Point::DrawingFunction, this, std::placeholders::_1, std::placeholders::_2);
    //p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Point::DrawingFunction);
}

void Point::DrawingFunction(QOpenGLContext *context, std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    //shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    shader->Bind();

    Renderer::DrawPoints(context->functions(), 1);
}
