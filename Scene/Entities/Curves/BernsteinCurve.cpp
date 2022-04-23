//
// Created by ksm on 4/22/22.
//

#include "BernsteinCurve.h"
#include "Scene/Utilities/Utilites.h"
#include "Renderer/Options.h"

BernsteinCurve::BernsteinCurve(unsigned int cid) : IEntity(cid)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));

    InitializeDrawing();

    CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);
    PolylineColor.setBinding([this](){
        return this->m_bezierPolyline.DrawingColor;
    });
    bezierPolylineDrawing = Options::DrawBezierPolygon.addNotifier([this]()
    {
        this->m_bezierPolyline.p_Drawing->Enabled = Options::DrawBezierPolygon;
    });
}

void BernsteinCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData({});
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BernsteinCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BernsteinCurve::UniformFunction);
}

void BernsteinCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BernsteinCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("m_Chunks", 64);//[TODO] to powinno byc liczone na shaderze
    shader->SetUniform("u_ObjectColor", ColorToVector4D(CurveColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

