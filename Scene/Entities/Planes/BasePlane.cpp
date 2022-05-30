//
// Created by ksm on 5/30/22.
//

#include "BasePlane.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"

BasePlane::BasePlane(uint cid, bool isPipe, int countU, int countV) : IEntity(cid)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));
    AddComponent(p_UV = UVParams::CreateRegisteredComponent(objectID, countU, countV));
    p_UV->UWraps = isPipe;

    InitializeDrawing();

    PlaneColor = DefaultColor;
    MeshColor.setBinding([this]()
    {
        return this->m_mesh.DrawingColor;
    });
    meshDrawingNotifier = Options::DrawPlainMesh.addNotifier([this]()
    {
        this->m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
    });
    meshColorNotifier = MeshColor.addNotifier([this]()
    {
        this->m_mesh.DrawingColor = MeshColor;
    });
    m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
}

BasePlane::~BasePlane()
{

}

void BasePlane::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData({});
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(PLANE_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BasePlane::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BasePlane::UniformFunction);
}

void BasePlane::DrawingFunction(QOpenGLContext *context)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );//rysowanie wireframow trojkatow wygenerowanych w shaderze teselacji
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BasePlane::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(PlaneColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->SetUniform("u_UDensity", p_UV->UDensity);
    shader->SetUniform("u_VDensity", p_UV->VDensity);
    shader->GetRawProgram()->setPatchVertexCount(16);
}
