//
// Created by ksm on 10/21/22.
//

#include "CutterPathPolyline.h"
#include "Scene/Utilities/Utilites.h"

CutterPathPolyline::CutterPathPolyline(const std::vector<QVector3D>& points, std::shared_ptr<Transform> simulatorTransform)
: IEntity(SIMULATOR_CUTTERPATH), m_points(points), m_simulatorTransform(simulatorTransform)
{
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
}

void CutterPathPolyline::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&CutterPathPolyline::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&CutterPathPolyline::UniformFunction);
}

void CutterPathPolyline::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLineStrip(context->functions(), m_points.size());
}

void CutterPathPolyline::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    QMatrix4x4 modelMtx;
    if (auto trans = m_simulatorTransform.lock())
        modelMtx = trans->GetModelMatrix();
    shader->SetUniform("u_MVP.Model", modelMtx);
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
}

std::vector<float> CutterPathPolyline::GenerateGeometryVertices()
{
    std::vector<float> res;
    res.reserve(m_points.size() * 3);
    
    for (auto el : m_points)
    {
        res.push_back(el.x());
        res.push_back(el.y());
        res.push_back(el.z());
    }
    
    return res;
}

std::vector<int> CutterPathPolyline::GenerateTopologyIndices()
{
    return {};
}

