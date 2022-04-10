//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "BezierC0.h"

BezierC0::BezierC0(): IEntity(BEZIERC0_CLASS)
{
    p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID);
    //BezierPoints = TransformCollection::CreateRegisteredComponent(objectID);

    InitializeDrawing();
    QObject::connect(BezierPolyline.p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &BezierC0::OnCollectionModified);
}

void BezierC0::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("m_Chunks",
            CalculateDrawableChunks(
                    Renderer::controller.viewport->GetProjectionMatrix(),
                    Renderer::controller.Camera->GetViewMatrix(),
                    Renderer::controller.viewport->GetViewportSize()
                    )
                );
    shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

void BezierC0::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BezierC0::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BezierC0::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BezierC0::UniformFunction);
}

std::vector<int> BezierC0::GenerateTopologyIndices()
{
    int groups = std::ceil(BezierPolyline.p_Collection->Size() / 3.0f);
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

std::vector<float> BezierC0::GenerateGeometryVertices()
{
    std::vector<float> res (4 * BezierPolyline.p_Collection->Size());

    int i = 0;
    for (const std::shared_ptr<Transform>& p : BezierPolyline.p_Collection->GetPoints())
    {
        res[4 * i] = (*p->Position).x();
        res[4 * i + 1] = (*p->Position).y();
        res[4 * i + 2] = (*p->Position).z();
        res[4 * i + 3] = 1.0f;
        i++;
    }

    return res;
}

void BezierC0::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
}

int BezierC0::GetIndexCount()
{
    return 4 * std::ceil(BezierPolyline.p_Collection->Size() / 3.0f);
}

int BezierC0::CalculateDrawableChunks(QMatrix4x4 proj, QMatrix4x4 view, QSize viewport)
{
    float maxX = FLT_MIN, maxY = FLT_MIN, minX = FLT_MAX, minY = FLT_MAX;
    for (const std::shared_ptr<Transform>& p : BezierPolyline.p_Collection->GetPoints())
    {
        QVector3D screenPoint = (*p->Position).project(view, proj, QRect(QPoint(), viewport));

        maxX = std::max(maxX, screenPoint.x());
        maxY = std::max(maxY, screenPoint.y());
        minX = std::min(minX, screenPoint.x());
        minY = std::min(minY, screenPoint.y());
    }

    int length = std::max(maxX - minX, maxY - minY);
    length /= 4;
    int res = std::min(std::max(4, length), 64);

    return res;
}
