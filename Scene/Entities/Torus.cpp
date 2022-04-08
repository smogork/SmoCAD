//
// Created by ksm on 4/5/22.
//

#include "Torus.h"

Torus::Torus(QVector3D position): IEntity(TORUS_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, position);
    p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID);
    p_UV = UVParams::CreateRegisteredComponent(objectID);

    InitializeDrawing();
    QObject::connect(p_UV.get(), &UVParams::ParametersChanged, this, &Torus::UVChanged);
}

void Torus::UVChanged(std::shared_ptr<UVParamsChanged> e)
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
}

void Torus::DrawingFunction(QOpenGLContext *context, std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_MVP.Model", p_Transform->GetModelMatrix());
    shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    shader->Bind();

    Renderer::DrawLines(context->functions(), GetIndexCount());
}

std::vector<float> Torus::GenerateGeometryVertices()
{
    std::vector<float> res(3 * p_UV->VDensity() * p_UV->UDensity());

    int vIndex = 0;
    for (int u = 0; u < p_UV->UDensity(); ++u)
    {
        float uDegree = u * 2.0f * M_PI / p_UV->UDensity();
        QMatrix4x4 rotY;
        rotY.rotate(uDegree * 180 / M_PI, Transform::GetYAxis());
        for (int v = 0; v < p_UV->VDensity(); ++v)
        {
            float vDegree = v * 2.0f * M_PI / p_UV->VDensity();
            QVector4D p = rotY * QVector4D(
                    p_UV->V() * cosf(vDegree) + p_UV->U(),
                    p_UV->V() * sinf(vDegree),
                    0.0f, 1.0f) ;

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
    for (int u = 0; u < p_UV->UDensity(); ++u)
    {
        int uOffset = u * p_UV->VDensity();

        //Mały okrąg
        for (int v = 0; v < p_UV->VDensity(); ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = uOffset + ((v + 1) % p_UV->VDensity());
            eIndex += 2;
        }

        //Połaczenie z kolejnym okręgiem modulo
        for (int v = 0; v < p_UV->VDensity(); ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = (uOffset + p_UV->VDensity() + v) % (p_UV->VDensity() * p_UV->UDensity());
            eIndex += 2;
        }
    }
    return res;
}

int Torus::GetIndexCount()
{
    return p_UV->UDensity() * p_UV->UDensity() * 4;
}


