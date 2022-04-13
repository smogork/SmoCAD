//
// Created by ksm on 4/9/22.
//

#include "Polyline.h"

Polyline::Polyline(): IEntity(POLYLINE_CLASS)
{
    p_Drawing = DynamicDrawing::CreateRegisteredComponent(objectID);
    p_Collection = TransformCollection::CreateRegisteredComponent(objectID);

    InitializeDrawing();
    QObject::connect(p_Collection.get(), &TransformCollection::PointInCollectionModified,
                     this, &Polyline::OnCollectionModified);
    QObject::connect(p_Collection.get(), &TransformCollection::SinglePointChanged,
                     this, &Polyline::OnSinglePointModified);
}

std::vector<float> Polyline::GenerateGeometryVertices()
{
    std::vector<float> res;
    res.reserve(p_Collection->Size() * 3);

    for (auto elw : p_Collection->GetPoints())
        if (auto el = elw.lock())
        {
            res.push_back((*el->Position).x());
            res.push_back((*el->Position).y());
            res.push_back((*el->Position).z());
        }

    return res;
}

std::vector<int> Polyline::GenerateTopologyIndices()
{
    return {};
}

void Polyline::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&Polyline::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&Polyline::UniformFunction);
}

void Polyline::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLineStrip(context->functions(), p_Collection->Size());
}

void Polyline::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", QVector4D(DrawingColor.redF(),
                                                  DrawingColor.greenF(),
                                                  DrawingColor.blueF(),
                                                  DrawingColor.alphaF()));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

void Polyline::OnCollectionModified()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
}

void Polyline::OnSinglePointModified(QVector3D pos, unsigned int changedOID)
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
}


