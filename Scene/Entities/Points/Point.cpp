//
// Created by ksm on 4/3/22.
//

#include "Point.h"
#include "Scene/Utilities/Utilites.h"

Point::Point(const QString& name, QVector3D pos): IEntity(POINT_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(objectID, pos));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(objectID));
    InitializeDrawing();
    AddComponent(p_Selectable = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_ScreenSelectable = ScreenSelectable::CreateRegisteredComponent(objectID, p_Transform, p_Selectable));
    AddComponent(p_CompositeAware = CompositeAware::CreateRegisteredComponent(objectID, p_Transform, p_Drawing));
    AddComponent(p_CollectionAware = CollectionAware::CreateRegisteredComponent(objectID, p_Transform));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selectable));

    selectedNotifier = p_Selectable->Selected.addNotifier([this]() {
       HandleColors();
    });
    compositeNotifier = p_CompositeAware->InsideComposite.addNotifier([this]() {
       HandleColors();
    });
}

Point::Point(const QString &name) : Point(name, QVector3D(0, 0, 0))
{ }

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

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Point::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&Point::UniformFunction);
}

void Point::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPoints(context->functions(), 1);
}


void Point::UniformFunction(std::shared_ptr<ShaderWrapper> shader) {
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", ColorToVector4D(m_color));
}

void Point::HandleColors()
{
    if (p_Selectable->Selected)
        m_color = Selectable::SelectedColor;
    else if (p_CompositeAware->InsideComposite)
        m_color = CompositeAware::CompositeColor;
    else
        m_color = DefaultColor;
}
