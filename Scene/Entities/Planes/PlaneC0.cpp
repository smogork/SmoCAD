//
// Created by ksm on 5/14/22.
//

#include "PlaneC0.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"

/*PlaneC0::PlaneC0(const QString& name, bool isPipe, int width, int height): IEntity(PLANEC0_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));
    if (isPipe)
        AddComponent(p_UV = UVParams::CreateRegisteredComponent(objectID, width/3.0f, (height - 1)/3.0f));
    else
        AddComponent(p_UV = UVParams::CreateRegisteredComponent(objectID, (width - 1)/3.0f, (height - 1)/3.0f));
    p_UV->UWraps = isPipe;

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension = width;//to musi zostac utworzone w roznych typach plaszczyzn
    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlaneC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlaneC0::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &PlaneC0::PointRemovedFromCollection);

    selectedNotifier = p_Selected->Selected.addNotifier([this]() {//to zostaje
        if (p_Selected->Selected)
            PlainColor = Selectable::SelectedColor;
        else
            PlainColor = DefaultColor;
    });

    meshColorNotifier = MeshColor.addNotifier([this]() {
        this->m_mesh.DrawingColor = MeshColor;
    });
    PlainColor = DefaultColor;
    MeshColor = Qt::darkGreen;//to trzeba ustawic w konkretnych plaszczyznach

    meshDrawingNotifier = Options::DrawPlainMesh.addNotifier([this]()
    {
        this->m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
    });
    m_mesh.p_Drawing->Enabled = Options::DrawPlainMesh;
}


PlaneC0::~PlaneC0()
{
}

std::vector<float> PlaneC0::GenerateGeometryVertices()
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

std::vector<int> PlaneC0::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    int res_idx = 0;

    int second_dim = (PATCH_SIZE - 1) * p_UV->U + 1;
    if (p_UV->UWraps)
        second_dim--;
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
            for (int i = 0; i < PATCH_SIZE; ++i)//height
                for (int j = 0; j < PATCH_SIZE; ++j)//width
                {
                    int wIdx = w * (PATCH_SIZE - 1) + j;
                    int hIdx = h * (PATCH_SIZE - 1) + i;
                    res[res_idx++] = hIdx * second_dim + (wIdx % second_dim);
                }

    return res;
}

int PlaneC0::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

void PlaneC0::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(PLANE_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&PlaneC0::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&PlaneC0::UniformFunction);
}

void PlaneC0::DrawingFunction(QOpenGLContext *context)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void PlaneC0::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(PlainColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->SetUniform("u_UDensity", p_UV->UDensity);
    shader->SetUniform("u_VDensity", p_UV->VDensity);
    shader->GetRawProgram()->setPatchVertexCount(16);
}*/

void PlaneC0::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC0::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_mesh.p_Collection) = (*p_Collection);
}

void PlaneC0::PointRemovedFromCollection()
{
    if (!IsDeleted())
        if (auto scene = SceneECS::Instance().lock())
            scene->RemoveObject(GetObjectID());
}

std::vector<float> PlaneC0::GenerateGeometryVertices()
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

std::vector<int> PlaneC0::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    int res_idx = 0;

    int second_dim = (PATCH_SIZE - 1) * p_UV->U + 1;
    if (p_UV->UWraps)
        second_dim--;
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
            for (int i = 0; i < PATCH_SIZE; ++i)//height
                for (int j = 0; j < PATCH_SIZE; ++j)//width
                {
                    int wIdx = w * (PATCH_SIZE - 1) + j;
                    int hIdx = h * (PATCH_SIZE - 1) + i;
                    res[res_idx++] = hIdx * second_dim + (wIdx % second_dim);
                }

    return res;
}

int PlaneC0::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

PlaneC0::PlaneC0(const QString &name, bool isPipe, int countU, int countV):
    BasePlane(PLANEC0_CLASS, isPipe, countU, countV)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension =
            isPipe ? (PATCH_SIZE - 1) * countU : (PATCH_SIZE - 1) * countU + 1;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlaneC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlaneC0::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &PlaneC0::PointRemovedFromCollection);

    selectedNotifier = p_Selected->Selected.addNotifier([this]() {
        if (p_Selected->Selected)
            PlaneColor = Selectable::SelectedColor;
        else
            PlaneColor = DefaultColor;
    });
    MeshColor = Qt::darkGreen;
}


