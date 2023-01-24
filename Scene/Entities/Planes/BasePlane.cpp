//
// Created by ksm on 5/30/22.
//

#include "BasePlane.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"

BasePlane::BasePlane(uint cid, bool isPipe, int countU, int countV) : IEntity(cid)
{
    InitObject(isPipe, countU, countV);
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
    shader->SetUniform("u_UPatch", p_UV->U);
    shader->SetUniform("u_VPatch", p_UV->V);
    shader->GetRawProgram()->setPatchVertexCount(16);
}

int BasePlane::GetPatchCount()
{
    return p_UV->U * p_UV->V;
}

BasePlane::BasePlane(unsigned int cid, uint explicit_oid, bool isPipe, int countU, int countV) : IEntity(cid, explicit_oid)
{
    InitObject(isPipe, countU, countV);
}

void BasePlane::InitObject(bool isPipe, int countU, int countV)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_UV = UVParams::CreateRegisteredComponent(GetObjectID(), countU, countV));
    m_mesh.IsPipe = isPipe;

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

void BasePlane::RemovePointsInside()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto points = p_Collection->GetPoints();
        p_Collection->Clear();
        for (const auto& wp : points)
        {
            if (auto p = wp.lock())
                scene->RemoveObject(p->GetAttachedObjectID());
        }
    }

}
