//
// Created by ksm on 11/9/22.
//

#include "RoutingAwareSystem.h"

void RoutingAwareSystem::StartHeighmapRendering()
{
    //Przygotowanie specjanych macierzy projekcji i widoku aby wyrenderowac mapę wysokości
    QMatrix4x4 ortoProj, viewMtx;
    QVector3D C = WorldStartPoint + QVector3D(0, (*BlockSize).z(), 0);

    ortoProj.ortho(
            -(*BlockSize).x() / 2, (*BlockSize).x() / 2,
            (*BlockSize).y() / 2, -(*BlockSize).y() / 2,
            0, (*BlockSize).z());
    viewMtx.lookAt(C, WorldStartPoint, Transform::GetYAxis());

    Renderer::SetUniformAtAllShaders("u_MVP.Projection", ortoProj);
    Renderer::SetUniformAtAllShaders("u_MVP.View", viewMtx);

    for (const auto &weak_comp: GetComponents())
        if (auto comp = weak_comp.lock())
            comp->StartHeighmapRendering();
}

void RoutingAwareSystem::FinishHeighmapRendering()
{
    //Przywrócenie macierzy projekcji oraz widoku do normalnego stanu
    Renderer::UpdateShaders();

    for (const auto &weak_comp: GetComponents())
        if (auto comp = weak_comp.lock())
            comp->FinishHeighmapRendering();
}

void RoutingAwareSystem::RenderHeightmap(QOpenGLContext* context)
{
    auto gl = context->functions();

    gl->glColorMask(true, true, true, true);
    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &weak_comp: GetComponents())
        if (auto comp = weak_comp.lock())
            comp->p_Drawing->Render(context);
}
