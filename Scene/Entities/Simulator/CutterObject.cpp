//
// Created by ksm on 10/20/22.
//

#include "CutterObject.h"
#include "Mathematics/PointShapes.h"

CutterObject::CutterObject(QVector3D pos, CutterParameters params, std::shared_ptr<Transform> simulatorTransform)
        : IEntity(SIMULATOR_CUTTER), m_params(params), m_simulatorTransform(simulatorTransform)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
}

void CutterObject::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(SIMULATOR_CUTTER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&CutterObject::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&CutterObject::UniformFunction);
}

void CutterObject::DrawingFunction(QOpenGLContext *context)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    Renderer::DrawTriangles(context->functions(), m_indexCount);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void CutterObject::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    QMatrix4x4 modelMtx = p_Transform->GetModelMatrix();
    if (auto trans = m_simulatorTransform.lock())
        modelMtx = trans->GetModelMatrix() * modelMtx;
    shader->SetUniform("u_MVP.Model", modelMtx);
}

std::vector<float> CutterObject::GenerateGeometryVertices()
{
    std::vector<float> res;

    std::vector<QVector3D> topCover = PointShapes::CreateCircle(QVector3D(0, CutterHeight, 0),
                                                                m_params.Diameter.GetSceneUnits() / 2,
                                                                VertexRCount, XZ);

    std::vector<QVector3D> cylinder, bottom;
    switch (m_params.Type)
    {
        case Cylindrical:
            cylinder =
                    PointShapes::CreateTube(QVector3D(), m_params.Diameter.GetSceneUnits() / 2, CutterHeight,
                                            VertexRCount, VertexLCount, XY);
            bottom = PointShapes::CreateCircle(QVector3D(), m_params.Diameter.GetSceneUnits() / 2,
                                               VertexRCount, XZ);
            break;
        case Spherical:
            cylinder =
                    PointShapes::CreateTube(QVector3D(0, m_params.Diameter.GetSceneUnits() / 2, 0),
                                            m_params.Diameter.GetSceneUnits() / 2,
                                            CutterHeight - m_params.Diameter.GetSceneUnits() / 2, VertexRCount,
                                            VertexLCount, XY);
            bottom = PointShapes::CreateHalfSphere(QVector3D(0, m_params.Diameter.GetSceneUnits() / 2, 0),
                                                   m_params.Diameter.GetSceneUnits() / 2,
                                                   VertexRCount, VertexThetaCount, XZ);
            break;
    }

    AppendToVertices(res, topCover);
    AppendToVertices(res, cylinder);
    AppendToVertices(res, bottom);
    return res;
}

std::vector<int> CutterObject::GenerateTopologyIndices()
{
    int offset = 0;
    std::vector<int> topCoverIndices = PointShapes::CircleTriangleIndices(VertexRCount, false, &offset);
    std::vector<int> tubeIndices = PointShapes::TubeTriangleIndices(VertexRCount, VertexLCount, false, &offset);
    std::vector<int> bottom;
    switch (m_params.Type)
    {
        case Cylindrical:
            bottom = PointShapes::CircleTriangleIndices(VertexRCount, true, &offset);
            break;
        case Spherical:
            bottom = PointShapes::HalfSphereTriangleIndices(VertexRCount, VertexThetaCount, false, &offset);
            break;
    }

    std::vector<int> res;
    res.insert(res.end(), topCoverIndices.begin(), topCoverIndices.end());
    res.insert(res.end(), tubeIndices.begin(), tubeIndices.end());
    res.insert(res.end(), bottom.begin(), bottom.end());

    m_indexCount = res.size();
    return res;
}

void CutterObject::AppendToVertices(std::vector<float> &vertices, const std::vector<QVector3D> points)
{
    for (const auto &p: points)
    {
        vertices.push_back(p.x());
        vertices.push_back(p.y());
        vertices.push_back(p.z());
    }

}

void CutterObject::SetCutterParameters(Length diameter, CutterType type)
{
    m_params.Diameter = diameter;
    m_params.Type = type;

    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

void CutterObject::SetCutterParameters(CutterParameters params)
{
    SetCutterParameters(params.Diameter, params.Type);
}
