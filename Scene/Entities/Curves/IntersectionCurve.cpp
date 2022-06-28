//
// Created by ksm on 6/19/22.
//

#include "IntersectionCurve.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"


IntersectionCurve::IntersectionCurve(const QString &name, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                                     std::shared_ptr<IntersectionAware> two, bool isCycle)
        : IEntity(INTERSECTION_CURVE_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    AddComponent(p_IntersectionRes = IntersectionResult::CreateRegisteredComponent(GetObjectID(), points, one, two, isCycle));

    InitializeDrawing();

    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                if (p_Selected->Selected)
                    DrawingColor = Selectable::SelectedColor;
                else
                    DrawingColor = DefaultColor;
            });

    connect(one.get(), &IComponent::ComponentDeleted, this, &IntersectionCurve::OnIntersectionAwareDeleted);
    connect(two.get(), &IComponent::ComponentDeleted, this, &IntersectionCurve::OnIntersectionAwareDeleted);
}

void IntersectionCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&IntersectionCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&IntersectionCurve::UniformFunction);
}

void IntersectionCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLineStrip(context->functions(), GetIndexCount());
}

void IntersectionCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

std::vector<float> IntersectionCurve::GenerateGeometryVertices()
{
    std::vector<float> res;
    res.reserve(p_IntersectionRes->GetScenePointsSize() * 3);

    for (const QVector3D &p: p_IntersectionRes->GetScenePoints())
    {
        res.push_back(p.x());
        res.push_back(p.y());
        res.push_back(p.z());
    }

    return res;
}

int IntersectionCurve::GetIndexCount()
{
    return p_IntersectionRes->GetScenePointsSize();
}

void IntersectionCurve::OnIntersectionAwareDeleted()
{
    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(GetObjectID());
}
