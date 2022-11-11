//
// Created by ksm on 11/9/22.
//

#include "RoutingAwareSystem.h"
#include "Scene/SceneECS.h"

void RoutingAwareSystem::StartHeighmapRendering(QVector3D blockWorldPos, QVector3D blockSize)
{
    //Przygotowanie specjanych macierzy projekcji i widoku aby wyrenderowac mapę wysokości
    QMatrix4x4 ortoProj, viewMtx;
    QVector3D C = blockWorldPos + QVector3D(0, blockSize.z(), 0);

    ortoProj.ortho(
            -blockSize.x() / 2, blockSize.x() / 2,
            -blockSize.y() / 2, blockSize.y() / 2,
            0, blockSize.z());
    viewMtx.lookAt(C, blockWorldPos, Transform::GetXAxis());//Zmiana wektora UP!

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

void RoutingAwareSystem::RenderHeightmap(QOpenGLContext *context)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(true, true, true, true);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &weak_comp: GetComponents())
        if (auto comp = weak_comp.lock())
            comp->p_Drawing->Render(context);
}

void
RoutingAwareSystem::GenerateRoutes3C(GLWidget *gl, const QString& folderName, QVector3D blockWorldPos, QVector3D blockSize,
                                     int offscreenSize)
{
    QSize texSize = {offscreenSize, offscreenSize};
    //ShaderWrapper zmapAnalizerShader("Shaders/Compute/zmapConfiguration.comp");
    ShaderWrapper zmapStampCreatorShader("Shaders/Compute/prepareCutterStamp.comp");

    // 1.Wygenerowanie zmapy obiektu w zadanym przedziale
    StartHeighmapRendering(blockWorldPos, blockSize);
    auto zmapTex = gl->DrawOffscreen(
            texSize,
            [this](QOpenGLContext *context)
            {
                this->RenderHeightmap(context);
            });
    FinishHeighmapRendering();


    gl->makeCurrent();
    // 2. Wykonanie mapy dozwolonych z dla obróbki zgrubnej
    int K16TexRadiusX = (unsigned int)std::ceil(K16_RADIUS * offscreenSize / blockSize.x());
    int K16TexRadiusY = (unsigned int)std::ceil(K16_RADIUS * offscreenSize / blockSize.y());
    QSize K16StampSize = QSize(K16TexRadiusX * 2, K16TexRadiusY * 2);
    auto stampTex =  gl->CreateFloatTexture32(K16StampSize);
    gl->glBindImageTexture( 0, stampTex->textureId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F );

    zmapStampCreatorShader.SetUniform("u_Cutter.Radius", K16_RADIUS);
    zmapStampCreatorShader.SetUniform("u_Cutter.TexRadiusX", K16TexRadiusX);
    zmapStampCreatorShader.SetUniform("u_Cutter.TexRadiusY", K16TexRadiusY);
    zmapStampCreatorShader.SetUniform("u_Cutter.isCylindrical", false);
    zmapStampCreatorShader.SetUniform("u_OutsideCutterVal", blockSize.z() * 10);// Wartosc znacznie wieksza niz blok

    zmapStampCreatorShader.Bind();
    stampTex->bind(0);
    gl->glDispatchCompute(K16StampSize.width(), K16StampSize.height(), 1);
    gl->glFinish();

    // Wyczyszczenie zasobów
    zmapTex->destroy();
    stampTex->destroy();

    gl->doneCurrent();
}
