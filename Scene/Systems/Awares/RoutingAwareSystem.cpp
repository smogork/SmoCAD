//
// Created by ksm on 11/9/22.
//

#include "RoutingAwareSystem.h"
#include "Scene/SceneECS.h"
#include "FileManagers/GCodeSaver.h"

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
    float heightDelta = 2.5f / 2;
    QVector3D startPoint1 = {
            blockSize.x() / 2 - K16_RADIUS / 2,
            blockSize.z() - heightDelta,
            blockSize.y() / 2};
    QVector3D startPoint2 = {
            -(blockSize.x() / 2 - K16_RADIUS / 2),
            blockSize.z() - 2 * heightDelta,
            blockSize.y() / 2};

    auto roughLayer1 = GenerateRoughZigZag(confMapK16, startPoint1,
                                           0.8f, blockSize, texSize, 0.1f, X, Negative);
    auto roughLayer2 = GenerateRoughZigZag(confMapK16, startPoint2,
                                           0.8f, blockSize, texSize, 0.1f, X, Positive);

    CutterPath roughPath(CutterParameters(Length::FromMilimeters(16), CutterType::Spherical));
    roughPath.Points.insert(roughPath.Points.end(), roughLayer1.begin(), roughLayer1.end());
    roughPath.Points.insert(roughPath.Points.end(), roughLayer2.begin(), roughLayer2.end());
    GCodeSaver::SaveCutterPath(folderName, roughPath, 1);

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

std::vector<QVector3D>
RoutingAwareSystem::GenerateRoughZigZag(const std::vector<float> &confMap, QVector3D startPoint, float w,
                                        QVector3D blockSize, QSize texSize, float tolerance, ZigZagVariable variable, ZigZagDirection direction)
{
    //Obróbka przesuwajac sie po X w strone ujemna
    QPoint texStartPoint = FromBlockToTex(QVector2D(startPoint.x(), startPoint.z()), texSize, blockSize);
    int texW = w / blockSize.x() * texSize.width();

    //Wygenerowanie zygzaka bez uwzglednienia dopuszczalnej wysokosci
    QSize planeSize = texSize + QSize(2 * texW, 2 * texW);
    auto zigzag = CreateZigZagPoints(texStartPoint + QPoint(texW, texW),
                                     texW, startPoint.y(), planeSize, variable, direction);

    //Weryfikacja wyskosci zygzaka i przeorbienie punktow na wspolrzedne textury
    for (int i = 0; i < zigzag.size(); ++i)
    {
        QPoint texPoint = zigzag[i].first - QPoint(texW, texW);
        float height = startPoint.y();
        //Uwaga na zamienione wspolrzedne!
        int idx = texPoint.x() * texSize.width() + texPoint.y();
        if (idx >= 0 && idx < confMap.size())//co najwyzej dwukrotnosc tolerancji bledu na powierzchni
            height = std::max(height, confMap[idx] + 2 * tolerance);


        zigzag[i].first = texPoint;
        zigzag[i].second = height;
    }

    //Optymalizacja sciezki - wyrzucenie prostych odcinkow oraz zdyskretyzowanie zbocz
    std::vector<QVector3D> optimisedPath;
    QPoint lastDir;
    QPoint lastP;
    for (std::pair<QPoint, float> p: zigzag)
    {
        QPoint dir = p.first - lastP;

        //Przypadek pierwszego punktu
        if (optimisedPath.size() == 0)
        {
            auto blockPos = FromTexToBlock(p.first, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), p.second, blockPos.y());
        }
        //Przypadek gdy chodzimy po plaszczyznie, ale zeszlismy z obiektu i Z nie jest do konca plaszczyzna
        else if (p.second == startPoint.y() && std::abs(p.second - optimisedPath.back().y()) > 0)
        {
            auto blockPos = FromTexToBlock(p.first, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), startPoint.y(), blockPos.y());
        }
            //Przypadek zmiany kierunku
        else if (lastDir != dir)
        {
            auto blockPos = FromTexToBlock(lastP, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), startPoint.y(), blockPos.y());
        }
            //przypadek duzego odchylenia w Z
        else if (std::abs(p.second - optimisedPath.back().y()) > tolerance / 2)
        {
            auto blockPos = FromTexToBlock(p.first, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), p.second, blockPos.y());
        }

        lastP = p.first;
        lastDir = dir;
    }
    auto blockPos = FromTexToBlock(lastP, texSize, blockSize);
    optimisedPath.emplace_back(blockPos.x(), startPoint.y(), blockPos.y());

    return optimisedPath;
}

QPoint RoutingAwareSystem::FromBlockToTex(QVector2D blockPoint, QSize texSize, QVector3D blockSize)
{
    auto normalizedSize = blockPoint / blockSize.toVector2D() + QVector2D(0.5f, 0.5f);
    return {(int) (normalizedSize.x() * texSize.width()), (int) (normalizedSize.y() * texSize.height())};
}

QVector2D RoutingAwareSystem::FromTexToBlock(QPoint texPoint, QSize texSize, QVector3D blockSize)
{
    QVector2D normalizedSize = {(float) texPoint.x() / texSize.width(), (float) texPoint.y() / texSize.height()};
    return (normalizedSize - QVector2D(0.5f, 0.5f)) * blockSize.toVector2D();
}

std::vector<std::pair<QPoint, float>>
RoutingAwareSystem::CreateZigZagPoints(QPoint startPoint, int width, float height, QSize planeSize,
                                       ZigZagVariable variable, ZigZagDirection direction)
{

    std::vector<std::pair<QPoint, float>> zigzag;

    if (variable == X)
    {
        bool negInner = startPoint.y() > (planeSize.height() / 2);
        for (int x = startPoint.x(); direction == Positive ? x < planeSize.width() : x >= 0; x += width * (direction == Positive ? 1 : -1))
        {
            for (int y = negInner ? planeSize.height() - 1 : 0;
                 negInner ? y >= 0 : y <= planeSize.height() - 1; y += (negInner ? -1 : 1))
            {
                zigzag.emplace_back(std::make_pair(QPoint(x, y), height));
            }

            if (direction == Positive ? x + width <= planeSize.width() : x - width >= 0)
            {
                for (int i = x +  (direction == Positive ? 1 : -1);
                     direction == Positive ? i < x + width : i > x - width; i += (direction == Positive ? 1 : -1))
                    zigzag.emplace_back(std::make_pair(QPoint(i, negInner ? 0 : planeSize.height() - 1), height));
            }
            negInner = !negInner;
        }
    } else
    {
        bool negInner = startPoint.x() > (planeSize.width() / 2);
        for (int y = startPoint.y(); direction == Positive ? y < planeSize.height() : y >= 0; y += width * (direction == Positive ? 1 : -1))
        {
            for (int x = negInner ? planeSize.width() - 1 : 0;
                 negInner ? x >= 0 : x <= planeSize.width() - 1; x += negInner ? -1 : 1)
                zigzag.emplace_back(std::make_pair(QPoint(x, y), height));

            if (direction == Positive ? y + width <= planeSize.height() : y - width >= 0)
            {
                for (int i = y + (direction == Positive ? 1 : -1);
                     direction == Positive ? i < y + width : i > y - width; i += (direction == Positive ? 1 : -1))
                    zigzag.emplace_back(std::make_pair(QPoint(negInner ? 0 : planeSize.width() - 1, i), height));
            }
            negInner = !negInner;
        }
    }

    return zigzag;
}
