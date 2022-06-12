//
// Created by ksm on 6/11/22.
//

#include "SelectRectangle.h"

std::vector<int> SelectRectangle::indices = GenerateTopologyIndices();

SelectRectangle::SelectRectangle(QRect pos) : IEntity(SELECT_RECT_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();

    m_areaChangedNotifier = SelectionArea.addNotifier(
            [this]
            {
                p_Drawing->SetVertexData(GenerateGeometryVertices());
            });

    SelectionArea = pos;
}

void SelectRectangle::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{ }

void SelectRectangle::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawTriangles(context->functions(), 6);
}

void SelectRectangle::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData(indices);
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(SHADERS::SELECT_RECT_SHADER).lock())
        p_Drawing->AttachShader(sh);
    p_Drawing->IsTransparent = true;

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&SelectRectangle::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&SelectRectangle::UniformFunction);
}

std::vector<int> SelectRectangle::GenerateTopologyIndices()
{
    return {0, 2, 1, 1, 2, 3};//counter clockwise?
    //return {0, 1, 2, 1, 3, 2};//clockwise?
}

std::vector<float> SelectRectangle::GenerateGeometryVertices()
{
    QVector3D points[4] = {
            {(float) ((*SelectionArea).topLeft().x()),
                                                           Renderer::controller.viewport->GetViewportSize().height() -
                                                           (float) (*SelectionArea).topLeft().y(),
                                                                                                       0.0f},
            {(float) ((*SelectionArea).topRight().x()),    Renderer::controller.viewport->GetViewportSize().height() -
                                                           (float) (*SelectionArea).topRight().y(),    0.0f},
            {(float) ((*SelectionArea).bottomLeft().x()),  Renderer::controller.viewport->GetViewportSize().height() -
                                                           (float) (*SelectionArea).bottomLeft().y(),  0.0f},
            {(float) ((*SelectionArea).bottomRight().x()), Renderer::controller.viewport->GetViewportSize().height() -
                                                           (float) (*SelectionArea).bottomRight().y(), 0.0f}
    };

    std::vector<float> res;

    for (const QVector3D &p: points)
    {
        /*QVector3D worldPoint = p.unproject(
                Renderer::controller.Camera->GetViewMatrix(),
                Renderer::controller.viewport->GetProjectionMatrix(),
                QRect(QPoint(0.0f, 0.0f), Renderer::controller.viewport->GetViewportSize()));

        QVector4D ndcPoint = Renderer::controller.viewport->GetProjectionMatrix() * Renderer::controller.Camera->GetViewMatrix() *
                QVector4D(worldPoint, 1.0f);
        ndcPoint /= ndcPoint.w();*/
    
        QVector3D ndcPoint = p.unproject(
                QMatrix4x4(),
                QMatrix4x4(),
                QRect(QPoint(0.0f, 0.0f), Renderer::controller.viewport->GetViewportSize()));
        
        res.push_back(ndcPoint.x());
        res.push_back(ndcPoint.y());
        res.push_back(ndcPoint.z());
    }

    return res;
}
