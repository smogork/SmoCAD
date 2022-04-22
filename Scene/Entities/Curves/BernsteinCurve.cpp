//
// Created by ksm on 4/22/22.
//

#include "BernsteinCurve.h"
#include "Scene/Utilities/Utilites.h"

BernsteinCurve::BernsteinCurve(unsigned int cid) : IEntity(cid)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(objectID));

    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BernsteinCurve::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &BernsteinCurve::OnSinglePointModified);

    PolylineColor.setBinding([this](){
        return this->m_bezierPolyline.DrawingColor;
    });
}

void BernsteinCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BernsteinCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BernsteinCurve::UniformFunction);
}

void BernsteinCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BernsteinCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(CurveColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

std::vector<int> BernsteinCurve::GenerateTopologyIndices()
{
    int groups = std::ceil(p_Collection->Size() / 3.0f);
    std::vector<int> res (4 * groups);

    for (int i = 0; i < groups; ++i)
    {
        res[4 * i] = 3 * i;
        res[4 * i + 1] = 3 * i + 1;
        res[4 * i + 2] = 3 * i + 2;
        res[4 * i + 3] = 3 * i + 3;
    }

    return res;
}

int BernsteinCurve::GetIndexCount()
{
    return 4 * std::ceil(p_Collection->Size() / 3.0f);
}

