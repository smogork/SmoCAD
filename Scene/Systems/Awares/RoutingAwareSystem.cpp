//
// Created by ksm on 11/9/22.
//

#include "RoutingAwareSystem.h"
#include "Scene/SceneECS.h"

void RoutingAwareSystem::ClearSystem()
{
    zmapStampCreatorShader.reset();
    zmapAnalizerShader.reset();
}

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
RoutingAwareSystem::GenerateRoutes3C(GLWidget *gl, const QString &folderName, QVector3D blockWorldPos,
                                     QVector3D blockSize,
                                     int offscreenSize)
{
    if (!zmapStampCreatorShader)
        zmapStampCreatorShader = std::make_unique<ShaderWrapper>("Shaders/Compute/prepareCutterStamp.comp");
    if (!zmapAnalizerShader)
        zmapAnalizerShader = std::make_unique<ShaderWrapper>("Shaders/Compute/zmapConfiguration.comp");

    QSize texSize = {offscreenSize, offscreenSize};


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
    auto confMapK16 = CreateConfigurationMap(gl, zmapTex, K16_RADIUS, offscreenSize, false, blockSize);
    DebugSaveConfMap(confMapK16, "confMapK16.png", texSize, blockSize.z());

    //3. Generowanie zygzakiem warstw dla obróbki zgrubnej

    // Wyczyszczenie zasobów
    zmapTex->destroy();
    gl->doneCurrent();
}

std::shared_ptr<QOpenGLTexture> RoutingAwareSystem::CreateStampTexture(
        GLWidget *gl, float radius, int offscreenSize, bool isCylindrical, QVector3D blockSize)
{
    int texRadiusX = (unsigned int) std::ceil(K16_RADIUS * offscreenSize / blockSize.x());
    int texRadiusY = (unsigned int) std::ceil(K16_RADIUS * offscreenSize / blockSize.y());
    QSize stampSize = QSize(texRadiusX * 2, texRadiusY * 2);
    auto stampTex = gl->CreateFloatTexture32(stampSize);
    gl->glBindImageTexture(0, stampTex->textureId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    zmapStampCreatorShader->SetUniform("u_Cutter.Radius", K16_RADIUS);
    zmapStampCreatorShader->SetUniform("u_Cutter.TexRadiusX", texRadiusX);
    zmapStampCreatorShader->SetUniform("u_Cutter.TexRadiusY", texRadiusY);
    zmapStampCreatorShader->SetUniform("u_Cutter.isCylindrical", isCylindrical);
    zmapStampCreatorShader->SetUniform("u_OutsideCutterVal", blockSize.z() * 10);// Wartosc znacznie wieksza niz blok

    zmapStampCreatorShader->Bind();
    stampTex->bind(0);
    gl->glDispatchCompute(stampSize.width(), stampSize.height(), 1);
    gl->glFinish();

    return stampTex;
}


std::vector<float>
RoutingAwareSystem::CreateConfigurationMap(GLWidget *gl, std::shared_ptr<QOpenGLTexture> heightTex,
                                           float radius, int offscreenSize, bool isCylindrical, QVector3D blockSize)
{
    auto confMapBuf = gl->CreateComputeBuffer<float>(offscreenSize * offscreenSize);

    int texRadiusX = (unsigned int) std::ceil(radius * offscreenSize / blockSize.x());
    int texRadiusY = (unsigned int) std::ceil(radius * offscreenSize / blockSize.y());
    zmapAnalizerShader->SetUniform("u_OutsideCutterVal", blockSize.z() * 10);// Wartosc znacznie wieksza niz blok
    zmapAnalizerShader->SetUniform("u_BlockHeight", blockSize.z());
    zmapAnalizerShader->SetUniform("u_Cutter.Radius", radius);
    zmapAnalizerShader->SetUniform("u_Cutter.TexRadiusX", texRadiusX);
    zmapAnalizerShader->SetUniform("u_Cutter.TexRadiusY", texRadiusY);
    zmapAnalizerShader->SetUniform("u_Cutter.isCylindrical", isCylindrical);
    zmapAnalizerShader->SetUniform("u_OutsideCutterVal", blockSize.z() * 10);

    zmapAnalizerShader->Bind();
    heightTex->bind(0);
    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, confMapBuf->bufferId());
    gl->glDispatchCompute(offscreenSize / 16, offscreenSize / 16, 1);
    gl->glFinish();

    std::vector<float> confMap(offscreenSize * offscreenSize);
    confMapBuf->bind();
    confMapBuf->read(0, confMap.data(), confMap.size() * sizeof(float));
    confMapBuf->release();
    confMapBuf->destroy();

    return confMap;
}

void RoutingAwareSystem::DebugSaveConfMap(const std::vector<float> &map, const QString &path, QSize texSize,
                                          float blockHeight)
{
    //Tutaj tylko do zgrubnego ogladania
    QImage confMapImg(texSize.width(), texSize.height(), QImage::Format::Format_ARGB32);
    for (int i = 0; i < map.size(); ++i)
        confMapImg.setPixelColor(i % texSize.width(), texSize.height() - i / texSize.width() - 1,
                                 QColor(map[i] / blockHeight * 255, 0, 0));
    confMapImg.save(path);
}
