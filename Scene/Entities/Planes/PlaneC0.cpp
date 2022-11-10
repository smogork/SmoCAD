//
// Created by ksm on 5/14/22.
//

#include "PlaneC0.h"
#include "Renderer/Options.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"
#include "Mathematics/Polynomials.h"
#include "ThirdParty/Scene-Serializer/cpp/Serializer/Serializer/Scene/SerializerException.h"
#include "Scene/Systems/Awares/CollectionAwareSystem.h"

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
    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(GetObjectID());
}

std::vector<float> PlaneC0::GenerateGeometryVertices()
{
    std::vector<float> res(3 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform> &pw: p_Collection->GetPoints())
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

    std::vector<int> patch_indices(16);
    for (int h = 0; h < p_UV->V; ++h)//height
        for (int w = 0; w < p_UV->U; ++w)//width
        {
            GetIndexesOfPatch(w, h, patch_indices);
            for (int idx: patch_indices)
                res[res_idx++] = idx;
        }

    return res;
}

int PlaneC0::GetIndexCount()
{
    return PATCH_SIZE * PATCH_SIZE * p_UV->U * p_UV->V;
}

PlaneC0::PlaneC0(const QString &name, bool isPipe, int countU, int countV) :
        BasePlane(PLANEC0_CLASS, isPipe, countU, countV)
{
    InitObject(name, isPipe, countU, countV);
}

std::vector<std::shared_ptr<Point>>
PlaneC0::CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width,
                              float height)
{
    std::vector<std::shared_ptr<Point>> res;

    if (auto scene = SceneECS::Instance().lock())
    {
        std::vector<QVector3D> positions;
        if (isPipe)
            positions = PointShapes::CreateTube(startPos, width, height, (PATCH_SIZE - 1) * U,
                                                (PATCH_SIZE - 1) * V + 1);
        else
            positions = PointShapes::CreateRect(startPos, width, height, (PATCH_SIZE - 1) * U + 1,
                                                (PATCH_SIZE - 1) * V + 1);

        for (int i = 0; i < positions.size(); ++i)
        {
            QString pName = QString("P_%0_%1").arg(name).arg(i);
            auto p = std::make_shared<Point>(pName, positions[i]);
            res.emplace_back(p);
            scene->AddObjectExplicitPosition(p);
        }
    }

    return res;
}

void PlaneC0::SerializingFunction(MG1::Scene &scene)
{
    MG1::BezierSurfaceC0 p0;
    p0.SetId(GetObjectID());
    p0.name = p_SceneElement->Name.value().toStdString();
    p0.uWrapped = p_Intersection->UWraps;
    p0.vWrapped = p_Intersection->VWraps;
    p0.size.x = p_UV->U;
    p0.size.y = p_UV->V;

    auto indices = GenerateTopologyIndices();
    std::vector<MG1::PointRef> points;
    for (const auto &wp: p_Collection->GetPoints())
        if (auto p = wp.lock())
            points.emplace_back(MG1::PointRef(p->GetAttachedObjectID()));

    for (int i = 0; i < GetPatchCount(); ++i)
    {
        MG1::BezierPatchC0 patch;

        patch.name = p_SceneElement->Name.value().toStdString();//[TODO] dodac inne nazwy dla patchy
        patch.samples.x = p_UV->UDensity;
        patch.samples.y = p_UV->VDensity;

        for (int j = 0; j < PATCH_SIZE * PATCH_SIZE; ++j)
        {
            patch.controlPoints.emplace_back(points[indices[i * (PATCH_SIZE * PATCH_SIZE) + j]]);
        }

        p0.patches.emplace_back(patch);
    }

    scene.surfacesC0.push_back(p0);
}

PlaneC0::PlaneC0(const MG1::BezierSurfaceC0 &p0) : BasePlane(PLANEC0_CLASS, p0.GetId(), p0.uWrapped, p0.size.x,
                                                             p0.size.y)
{
    InitObject(p0.name.c_str(), p0.uWrapped, p0.size.x, p0.size.y);

    p_UV->LockEditUV(false);
    p_Collection->LockContent(false);
    p_Intersection->VWraps = p0.vWrapped;

    if (p0.patches.size() == 0)
        throw MG1::SerializerException("Error while deserializing Surface without patches");

    p_UV->UDensity = p0.patches[0].samples.x;
    p_UV->VDensity = p0.patches[0].samples.y;
    p_UV->LockEditUV();


    auto indices = GenerateTopologyIndices();
    std::vector<std::shared_ptr<CollectionAware>> points(GetVertexCount(p_Intersection->UWraps));

    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < GetPatchCount(); ++i)
        {
            MG1::BezierPatchC0 patch = p0.patches[i];

            for (int j = 0; j < PATCH_SIZE * PATCH_SIZE; ++j)
            {
                int idx = indices[i * (PATCH_SIZE * PATCH_SIZE) + j];//index punktu w kolekcji punktow
                if (!points[idx])//jeżeli jeszcze punkt nie został ustalony
                {
                    MG1::PointRef ref = patch.controlPoints[j];

                    if (auto el = scene->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(
                            ref.GetId()).lock())
                        points[idx] = el;//przypisanie referencji do odpowiedniego miejsca w powierzchni
                    else
                        throw MG1::SerializerException("Unknown point during deserialization of SurfaceC0");
                }
            }
        }

        p_Collection->SetPoints(points);
        p_Collection->LockContent();
    }
}

int PlaneC0::GetVertexCount(bool isPipe)
{
    if (isPipe)
        return ((PATCH_SIZE - 1) * p_UV->U) * ((PATCH_SIZE - 1) * p_UV->V + 1);
    return ((PATCH_SIZE - 1) * p_UV->U + 1) * ((PATCH_SIZE - 1) * p_UV->V + 1);
}

void PlaneC0::InitObject(const QString &name, bool isPipe, int countU, int countV)
{
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    AddComponent(p_FillAware = FillAware::CreateRegisteredComponent(GetObjectID(), p_Collection));
    AddComponent(p_Intersection = IntersectionAware::CreateRegisteredComponent(GetObjectID(), p_UV));
    AddComponent(p_Routing = RoutingAware::CreateRegisteredComponent(GetObjectID(), p_UV, p_Drawing));
    InitializeUV(isPipe);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&PlaneC0::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&PlaneC0::UniformFunction);
    //p_Drawing->IsTransparent = true;

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&PlaneC0::SerializingFunction);

    p_Collection->SecondDimension = m_mesh.p_Collection->SecondDimension =
            isPipe ? (PATCH_SIZE - 1) * countU : (PATCH_SIZE - 1) * countU + 1;

    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &PlaneC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &PlaneC0::OnSinglePointModified);
    QObject::connect(p_Collection.get(), &TransformCollection::PointDeleted,
                     this, &PlaneC0::PointRemovedFromCollection);

    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                if (p_Selected->Selected)
                    PlaneColor = Selectable::SelectedColor;
                else
                    PlaneColor = DefaultColor;
            });
    MeshColor = Qt::darkGreen;
}

void PlaneC0::InitializeUV(bool isPipe)
{
    p_Intersection->SceneFunction = ASSIGN_UV_FUNCTION(&PlaneC0::PlaneC0Func);
    p_Intersection->SceneFunctionDerU = ASSIGN_UV_FUNCTION(&PlaneC0::PlaneC0FuncDerU);
    p_Intersection->SceneFunctionDerV = ASSIGN_UV_FUNCTION(&PlaneC0::PlaneC0FuncDerV);

    p_Intersection->UWraps = isPipe;
    p_Intersection->VWraps = false;
    p_Intersection->UMin = 0;
    p_Intersection->VMin = 0;
    p_Intersection->UMax = p_UV->U.value();
    p_Intersection->VMax = p_UV->V.value();
}

QVector3D PlaneC0::PlaneC0Func(QVector2D uv)
{
    if (!p_Intersection->ArgumentsInsideDomain(uv))
        return {0, 0, 0};

    uv = p_Intersection->WrapArgumentsAround(uv);
    int planeNumU = (int) uv.x();
    int planeNumV = (int) uv.y();

    float u = fmodf(uv.x(), 1);
    float v = fmodf(uv.y(), 1);

    std::vector<int> patch_indices(16);
    std::vector<QVector3D> patch_points(16);
    std::vector<QVector3D> control_points(4), mid_values(4);
    GetIndexesOfPatch(planeNumU, planeNumV, patch_indices);

    auto points = p_Collection->GetVectorCoords();
    for (int i = 0; i < 16; ++i)
        patch_points[i] = points[patch_indices[i]];

    for (int i = 0; i < 4; ++i)
    {
        Polynomials::LoadControlPointsRow(i, patch_points, control_points);
        mid_values[i] = Polynomials::deCasteljau(u, control_points);
    }
    QVector3D pos = Polynomials::deCasteljau(v, mid_values);

    return pos;
}


QVector3D PlaneC0::PlaneC0FuncDerU(QVector2D uv)
{
    if (!p_Intersection->ArgumentsInsideDomain(uv))
        return {0, 0, 0};

    uv = p_Intersection->WrapArgumentsAround(uv);
    int planeNumU = (int) uv.x();
    int planeNumV = (int) uv.y();

    float u = fmodf(uv.x(), 1);
    float v = fmodf(uv.y(), 1);

    std::vector<int> patch_indices(16);
    std::vector<QVector3D> patch_points(16);
    std::vector<QVector3D> control_points(4), mid_values(4);
    GetIndexesOfPatch(planeNumU, planeNumV, patch_indices);

    auto points = p_Collection->GetVectorCoords();
    for (int i = 0; i < 16; ++i)
        patch_points[i] = points[patch_indices[i]];

    for (int i = 0; i < 4; ++i)
    {
        Polynomials::LoadControlPointsCol(i, patch_points, control_points);
        mid_values[i] = Polynomials::deCasteljau(v, control_points);
    }
    auto derUPoly = Polynomials::deCasteljauDerK(1, mid_values);
    QVector3D derU = Polynomials::deCasteljau(u, derUPoly);

    return derU;
}

QVector3D PlaneC0::PlaneC0FuncDerV(QVector2D uv)
{
    if (!p_Intersection->ArgumentsInsideDomain(uv))
        return {0, 0, 0};

    uv = p_Intersection->WrapArgumentsAround(uv);
    int planeNumU = (int) uv.x();
    int planeNumV = (int) uv.y();

    float u = fmodf(uv.x(), 1);
    float v = fmodf(uv.y(), 1);

    std::vector<int> patch_indices(16);
    std::vector<QVector3D> patch_points(16);
    std::vector<QVector3D> control_points(4), mid_values(4);
    GetIndexesOfPatch(planeNumU, planeNumV, patch_indices);

    auto points = p_Collection->GetVectorCoords();
    for (int i = 0; i < 16; ++i)
        patch_points[i] = points[patch_indices[i]];

    for (int i = 0; i < 4; ++i)
    {
        Polynomials::LoadControlPointsRow(i, patch_points, control_points);
        mid_values[i] = Polynomials::deCasteljau(u, control_points);
    }
    auto derVPoly = Polynomials::deCasteljauDerK(1, mid_values);
    QVector3D derV = Polynomials::deCasteljau(v, derVPoly);

    return derV;
}

void PlaneC0::GetIndexesOfPatch(int uPatch, int vPatch, std::vector<int> &indices)
{
    int index_width = (PATCH_SIZE - 1) * p_UV->U + 1;
    if (p_Intersection->UWraps)
        index_width--;

    int idx_counter = 0;
    for (int i = 0; i < PATCH_SIZE; ++i)//height
        for (int j = 0; j < PATCH_SIZE; ++j)//width
        {
            int wIdx = uPatch * (PATCH_SIZE - 1) + j;
            int hIdx = vPatch * (PATCH_SIZE - 1) + i;
            indices[idx_counter++] = hIdx * index_width + (wIdx % index_width);
        }
}

void PlaneC0::DrawingFunction(QOpenGLContext *context)
{
    //BasePlane::DrawingFunction(context);

    if (p_Routing->IsHeightmapRendering())
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );//rysowanie wireframow trojkatow wygenerowanych w shaderze teselacji
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void PlaneC0::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    BasePlane::UniformFunction(shader);

    p_Intersection->SetTrimmingUniforms(shader);
    if (p_Routing->IsHeightmapRendering())
    {
        shader->SetUniform("u_UDensity", 64);
        shader->SetUniform("u_VDensity", 64);
    }
}



