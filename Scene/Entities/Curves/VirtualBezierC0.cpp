//
// Created by ksm on 4/10/22.
//

#include <cfloat>
#include "VirtualBezierC0.h"

#include "Renderer/Options.h"

VirtualBezierC0::VirtualBezierC0(): IEntity(BEZIERC0_CLASS)
{
    InitObject();
}

void VirtualBezierC0::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", QVector4D(m_color.redF(),
                                                  m_color.greenF(),
                                                  m_color.blueF(),
                                                  m_color.alphaF()));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->GetRawProgram()->setPatchVertexCount(4);
}

void VirtualBezierC0::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void VirtualBezierC0::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&VirtualBezierC0::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&VirtualBezierC0::UniformFunction);
}

std::vector<int> VirtualBezierC0::GenerateTopologyIndices()
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

std::vector<float> VirtualBezierC0::GenerateGeometryVertices()
{
    std::vector<float> res (4 * p_Collection->Size());

    int i = 0;
    for (const std::weak_ptr<Transform>& pw : p_Collection->GetPoints())
        if (auto p = pw.lock())
        {
            res[4 * i] = (*p->Position).x();
            res[4 * i + 1] = (*p->Position).y();
            res[4 * i + 2] = (*p->Position).z();
            res[4 * i + 3] = 1.0f;
            i++;
        }

    return res;
}

void VirtualBezierC0::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_bezierPolyline.p_Collection) = (*p_Collection);
}

int VirtualBezierC0::GetIndexCount()
{
    return 4 * std::ceil(p_Collection->Size() / 3.0f);
}

int VirtualBezierC0::CalculateDrawableChunks(QMatrix4x4 proj, QMatrix4x4 view, QSize viewport)
{
    float maxX = FLT_MIN, maxY = FLT_MIN, minX = FLT_MAX, minY = FLT_MAX;
    for (const std::weak_ptr<Transform>& pw : p_Collection->GetPoints())
        if (auto p = pw.lock())
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

void VirtualBezierC0::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    (*m_bezierPolyline.p_Collection) = (*p_Collection);
}

VirtualBezierC0::VirtualBezierC0(uint oid) : IEntity(BEZIERC0_CLASS, oid)
{
    InitObject();
}

void VirtualBezierC0::InitObject()
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(GetObjectID()));

    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &VirtualBezierC0::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &VirtualBezierC0::OnSinglePointModified);


    polylineColorNotify = PolylineColor.addNotifier([this]() {
        this->m_bezierPolyline.DrawingColor = PolylineColor;
    });
    curveColorNotify = CurveColor.addNotifier([this](){
        this->m_color = CurveColor;
    });
    CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);

    bezierPolylineDrawing = Options::DrawBezierPolygon.addNotifier([this]()
                                                                   {
                                                                       this->m_bezierPolyline.p_Drawing->Enabled = Options::DrawBezierPolygon;
                                                                   });
    m_bezierPolyline.p_Drawing->Enabled = Options::DrawBezierPolygon;
}
