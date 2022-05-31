//
// Created by ksm on 4/22/22.
//

#include "BaseCurve.h"
#include "Scene/Utilities/Utilites.h"
#include "Renderer/Options.h"

BaseCurve::BaseCurve(unsigned int cid) : IEntity(cid)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));

    InitializeDrawing();

    CurveColor = DefaultColor;
    PolylineColor.setBinding([this](){
        return this->m_curvePolyline.DrawingColor;
    });
    curvePolylineDrawing = Options::DrawBezierPolygon.addNotifier([this]()
    {
        this->m_curvePolyline.p_Drawing->Enabled = Options::DrawBezierPolygon;
    });
    curvePolylineColor = PolylineColor.addNotifier([this]()
    {
       this->m_curvePolyline.DrawingColor = PolylineColor;
    });

}

void BaseCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData({});
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BaseCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BaseCurve::UniformFunction);
}

void BaseCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BaseCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(CurveColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->GetRawProgram()->setPatchVertexCount(4);
}

