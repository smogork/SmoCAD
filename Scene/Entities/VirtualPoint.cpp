//
// Created by ksm on 4/10/22.
//

#include "VirtualPoint.h"

VirtualPoint::VirtualPoint(QVector3D pos): IEntity(POINT_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, pos);
    p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID);
    InitializeDrawing();
    p_Selectable = Selectable::CreateRegisteredComponent(objectID, p_Transform);
    p_CollectionAware = CollectionAware::CreateRegisteredComponent(objectID, p_Transform);

    QObject::connect(p_Selectable.get(), &Selectable::SelectedChanged, this, &VirtualPoint::SelectedChanged);
}

void VirtualPoint::InitializeDrawing()
{
    p_Drawing->SetVertexData({0.0f, 0.0f, 0.0f});
    p_Drawing->SetIndexData({0});
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&VirtualPoint::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&VirtualPoint::UniformFunction);
}

void VirtualPoint::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPoints(context->functions(), 1);
}


void VirtualPoint::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", m_color);
}

void VirtualPoint::SelectedChanged(std::shared_ptr<SelectionChanged> e)
{
    if (e->Selected)
        m_color = QVector4D(1.0f, 0.5f, 0.2f, 1.0f);
    else
        m_color = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
}