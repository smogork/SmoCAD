//
// Created by ksm on 4/10/22.
//

#include "VirtualPoint.h"

VirtualPoint::VirtualPoint(QVector3D pos): IEntity(VIRTUAL_POINT_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(objectID, pos));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID));
    InitializeDrawing();
    AddComponent(p_Selectable = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_ScreenSelectable = ScreenSelectable::CreateRegisteredComponent(objectID, p_Transform, p_Selectable));
    AddComponent(p_CollectionAware = CollectionAware::CreateRegisteredComponent(objectID, p_Transform));

    selectedNotifier = p_Selectable->Selected.addNotifier([this]() {
        if (p_Selectable->Selected)
            m_color = QVector4D(1.0f, 0.5f, 0.2f, 1.0f);
        else
            m_color = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    });
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