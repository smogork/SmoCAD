//
// Created by ksm on 6/19/22.
//

#include "IntersectionCurve.h"
#include "Scene/Utilities/Utilites.h"

IntersectionCurve::IntersectionCurve(const QString &name, const std::vector<QVector2D> &intersectionPoints,
                                     std::function<QVector3D(QVector2D args)> sceneFunction) : IEntity(
        INTERSECTION_CURVE_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));

    m_paramPoints = intersectionPoints;
    m_pointCount = intersectionPoints.size();
    m_sceneFunction = sceneFunction;

    InitializeDrawing();

    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                if (p_Selected->Selected)
                    DrawingColor = Selectable::SelectedColor;
                else
                    DrawingColor = DefaultColor;
            });

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
    Renderer::DrawLineStrip(context->functions(), m_pointCount);
}

void IntersectionCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

std::vector<float> IntersectionCurve::GenerateGeometryVertices()
{
    std::vector<float> res;
    res.reserve(m_paramPoints.size() * 3);

    for (const QVector2D &arg: m_paramPoints)
    {
        auto p = m_sceneFunction(arg);
        res.push_back(p.x());
        res.push_back(p.y());
        res.push_back(p.z());
    }

    return res;
}
