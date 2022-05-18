//
// Created by ksm on 5/14/22.
//

#include "PlainC0.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"

PlainC0::PlainC0(const QString& name, int width, int height): IEntity(PLAINC0_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));
    AddComponent(p_UV = UVParams::CreateRegisteredComponent(objectID, (width - 1)/3.0f, (height - 1)/3.0f));

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension = width;
    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlainC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlainC0::OnSinglePointModified);


    meshColorNotifier = MeshColor.addNotifier([this]() {
        this->m_mesh.DrawingColor = MeshColor;
    });
    PlainColor = DefaultColor;

    meshDrawingNotifier = Options::DrawPlainMesh.addNotifier([this]()
    {
        this->m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
    });
    m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
}

std::vector<float> PlainC0::GenerateGeometryVertices()
{
    std::vector<float> res (3 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform>& pw : p_Collection->GetPoints())
        if (auto p = pw.lock())
        {
            res[3 * i] = (*p->Position).x();
            res[3 * i + 1] = (*p->Position).y();
            res[3 * i + 2] = (*p->Position).z();
            i++;
        }

    return res;
}

std::vector<int> PlainC0::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    int res_idx = 0;

    int second_dim = (PATCH_SIZE - 1) * p_UV->U + 1;
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
            for (int i = 0; i < PATCH_SIZE; ++i)//height
                for (int j = 0; j < PATCH_SIZE; ++j)//width
                {
                    int wIdx = w * (PATCH_SIZE - 1) + j;
                    int hIdx = h * (PATCH_SIZE - 1) + i;
                    res[res_idx++] = hIdx * second_dim + wIdx;
                }

    return res;
}

int PlainC0::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

void PlainC0::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(PLAIN_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&PlainC0::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&PlainC0::UniformFunction);
}

void PlainC0::DrawingFunction(QOpenGLContext *context)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void PlainC0::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(PlainColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

void PlainC0::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlainC0::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

