//
// Created by ksm on 4/3/22.
//

#include "Point.h"

Point::Point(QVector3D pos): IEntity(POINT_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, pos);
    p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID);
    InitializeDrawing();
    p_Selectable = Selectable::CreateRegisteredComponent(objectID, p_Transform);

    QObject::connect(p_Selectable.get(), &Selectable::SelectedChanged, this, &Point::SelectedChanged);
}

Point::~Point()
{
}

void Point::InitializeDrawing()
{
    p_Drawing->SetVertexData({0.0f, 0.0f, 0.0f});
    p_Drawing->SetIndexData({0});
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    //https://stackoverflow.com/a/7582576/18323924
    p_Drawing->p_renderingFunction = std::bind(&Point::DrawingFunction, this, std::placeholders::_1, std::placeholders::_2);
    //p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Point::DrawingFunction);
}

void Point::DrawingFunction(QOpenGLContext *context, std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", m_color);
    shader->Bind();

    Renderer::DrawPoints(context->functions(), 1);
}

void Point::SelectedChanged(std::shared_ptr<SelectionChanged> e)
{
    if (e->Selected)
        m_color = QVector4D(1.0f, 0.5f, 0.2f, 1.0f);
    else
        m_color = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
}
