//
// Created by ksm on 4/5/22.
//

#include "Torus.h"
#include "Scene/Utilities/Utilites.h"

Torus::Torus(const QString &name, QVector3D position) : IEntity(TORUS_CLASS)
{
    InitObject(name, position);
}

Torus::Torus(const QString &name) : Torus(name, QVector3D()) {}

void Torus::UVChanged()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

void Torus::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Torus::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&Torus::UniformFunction);
}

void Torus::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLines(context->functions(), GetIndexCount());
}

void Torus::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", ColorToVector4D(m_color));
}

std::vector<float> Torus::GenerateGeometryVertices()
{
    std::vector<float> res(3 * p_UV->VDensity * p_UV->UDensity);
    
    int vIndex = 0;
    for (int u = 0; u < p_UV->UDensity; ++u)
    {
        float uDegree = u * 2.0f * M_PI / p_UV->UDensity;
        /*QMatrix4x4 rotY;
        rotY.rotate(uDegree * 180 / M_PI, Transform::GetYAxis());*/
        for (int v = 0; v < p_UV->VDensity; ++v)
        {
            float vDegree = v * 2.0f * M_PI / p_UV->VDensity;
            /*QVector4D p = rotY * QVector4D(
                    p_UV->V * cosf(vDegree) + p_UV->U,
                    p_UV->V * sinf(vDegree),
                    0.0f, 1.0f);*/
            QVector3D p = TorusFunc({uDegree, vDegree});
            
            res[vIndex] = p.x();
            res[vIndex + 1] = p.y();
            res[vIndex + 2] = p.z();
            vIndex += 3;
        }
    }
    
    return res;
}

std::vector<int> Torus::GenerateTopologyIndices()
{
    std::vector<int> res(GetIndexCount());
    
    int eIndex = 0;
    for (int u = 0; u < p_UV->UDensity; ++u)
    {
        int uOffset = u * p_UV->VDensity;
        
        //Mały okrąg
        for (int v = 0; v < p_UV->VDensity; ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = uOffset + ((v + 1) % p_UV->VDensity);
            eIndex += 2;
        }
        
        //Połaczenie z kolejnym okręgiem modulo
        for (int v = 0; v < p_UV->VDensity; ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = (uOffset + p_UV->VDensity + v) % (p_UV->VDensity * p_UV->UDensity);
            eIndex += 2;
        }
    }
    return res;
}

int Torus::GetIndexCount()
{
    return p_UV->UDensity * p_UV->VDensity * 4;
}

void Torus::HandleColors()
{
    if (p_Selected->Selected)
        m_color = Selectable::SelectedColor;
    else if (p_CompositeAware->InsideComposite)
        m_color = CompositeAware::CompositeColor;
    else
        m_color = DefaultColor;
}

void Torus::SerializingFunction(MG1::Scene &scene)
{
    MG1::Torus t;
    t.SetId(GetObjectID());
    t.name = p_SceneElement->Name.value().toStdString();
    t.position = SerializeQVector3D(p_Transform->Position);
    t.rotation = SerializeQVector3D(p_Transform->Rotation);
    t.scale = SerializeQVector3D(p_Transform->Scale);
    t.largeRadius = p_UV->U;//R
    t.smallRadius = p_UV->V;//r
    t.samples.x = p_UV->UDensity;//R density
    t.samples.y = p_UV->VDensity;//r density
    
    scene.tori.push_back(t);
}

Torus::Torus(const MG1::Torus &serializedObj) : IEntity(TORUS_CLASS, serializedObj.GetId())
{
    InitObject(serializedObj.name.c_str(), DeserializeFloat3(serializedObj.position));
    
    p_Transform->Rotation = DeserializeFloat3(serializedObj.rotation);
    p_Transform->Scale = DeserializeFloat3(serializedObj.scale);
    p_UV->U = serializedObj.largeRadius;
    p_UV->V = serializedObj.smallRadius;
    p_UV->UDensity = serializedObj.samples.x;
    p_UV->VDensity = serializedObj.samples.y;
}

void Torus::InitObject(const QString &name, QVector3D position)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), position));
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_UV = UVParams::CreateRegisteredComponent(GetObjectID(), 5, 1));
    AddComponent(p_CompositeAware = CompositeAware::CreateRegisteredComponent(GetObjectID(), p_Transform, p_Drawing));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    AddComponent(p_Intersection = IntersectionAware::CreateRegisteredComponent(GetObjectID(), p_UV));
    
    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&Torus::SerializingFunction);
    
    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                HandleColors();
            });
    compositeNotifier = p_CompositeAware->InsideComposite.addNotifier(
            [this]()
            {
                HandleColors();
            });
    
    InitializeDrawing();
    InitializeUV();
}

void Torus::InitializeUV()
{
    p_Intersection->SceneFunction = ASSIGN_UV_FUNCTION(&Torus::TorusFunc);
    p_Intersection->SceneFunctionDerU = ASSIGN_UV_FUNCTION(&Torus::TorusFuncDerU);
    p_Intersection->SceneFunctionDerV = ASSIGN_UV_FUNCTION(&Torus::TorusFuncDerV);

    p_Intersection->UWraps = true;
    p_Intersection->VWraps = true;
    p_Intersection->UMin = 0.0f;
    p_Intersection->UMax = 2 * M_PIf;
    p_Intersection->VMin = 0.0f;
    p_Intersection->VMax = 2 * M_PIf;

    uNotifier = p_UV->U.addNotifier(ASSIGN_NOTIFIER_FUNCTION(&Torus::UVChanged));
    vNotifier = p_UV->V.addNotifier(ASSIGN_NOTIFIER_FUNCTION(&Torus::UVChanged));
    udNotifier = p_UV->UDensity.addNotifier(ASSIGN_NOTIFIER_FUNCTION(&Torus::UVChanged));
    vdNotifier = p_UV->VDensity.addNotifier(ASSIGN_NOTIFIER_FUNCTION(&Torus::UVChanged));
}

QVector3D Torus::TorusFunc(QVector2D uv)
{

    return (p_Transform->GetModelMatrix() *
            QVector4D(
            (p_UV->U + p_UV->V * cos(uv.y())) * cos(uv.x()),
            p_UV->V * sin(uv.y()),
            (p_UV->U + p_UV->V * cos(uv.y())) * sin(uv.x()),
            1)).toVector3D();
}

QVector3D Torus::TorusFuncDerU(QVector2D uv)
{
    return (p_Transform->GetModelMatrix() *
            QVector4D(
            -(p_UV->U + p_UV->V * cos(uv.y())) * sin(uv.x()),
            p_UV->V * sin(uv.y()),
            (p_UV->U + p_UV->V * cos(uv.y())) * cos(uv.x()),
            0)).toVector3D();
}

QVector3D Torus::TorusFuncDerV(QVector2D uv)
{
    return (p_Transform->GetModelMatrix() *
            QVector4D(
            - p_UV->V * sin(uv.y()) * cos(uv.x()),
            p_UV->V * cos(uv.y()),
            - p_UV->V * sin(uv.y()) * sin(uv.x()),
            0)).toVector3D();
}


