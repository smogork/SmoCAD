//
// Created by ksm on 11/9/22.
//

#include "RoutingAwareSystem.h"
#include "Scene/SceneECS.h"
#include "FileManagers/GCodeSaver.h"
#include "Scene/Entities/Planes/PlaneCreator.h"
#include "Scene/Entities/Planes/PlaneC2.h"
#include "Scene/Entities/Planes/OffsetPlane.h"
#include "IntersectionAwareSystem.h"
#include "Mathematics/PlaneDivision.h"

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
    const float BottomDepth = 2.5f;


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
    auto confMapF12 = CreateConfigurationMap(gl, zmapTex, F12_RADIUS * 1.3f, offscreenSize, true, blockSize);
    DebugSaveConfMap(confMapK16, "confMapK16.png", texSize, blockSize.z());
    DebugSaveConfMap(confMapF12, "confMapF12.png", texSize, blockSize.z());

    //3. Generowanie zygzakiem warstw dla obróbki zgrubnej
    float heightDelta = BottomDepth / 2;
    QVector3D startPoint11 = {
            blockSize.x() / 2 - K16_RADIUS / 2,
            blockSize.z() - heightDelta,
            blockSize.y() / 2};
    QVector3D startPoint12 = {
            -(blockSize.x() / 2 - K16_RADIUS / 2),
            blockSize.z() - 2 * heightDelta,
            blockSize.y() / 2};

    auto roughLayer1 = GenerateRoughZigZag(confMapK16, startPoint11,
                                           0.8f, blockSize, texSize, 0.1f, X, Negative);
    auto roughLayer2 = GenerateRoughZigZag(confMapK16, startPoint12,
                                           0.8f, blockSize, texSize, 0.1f, X, Positive);

    CutterPath roughPath(CutterParameters(Length::FromMilimeters(16), CutterType::Spherical));
    roughPath.Points.insert(roughPath.Points.end(), roughLayer1.begin(), roughLayer1.end());
    roughPath.Points.insert(roughPath.Points.end(), roughLayer2.begin(), roughLayer2.end());
    GCodeSaver::SaveCutterPath(folderName, roughPath, 1);

    // 4.Generowanie obrobki zgrubnej dla plaskiej podstawki
    QVector3D startPoint21 = {
            blockSize.x() / 2 ,
            blockSize.z() - BottomDepth,
            -blockSize.y() / 2 - 1.2f * F12_RADIUS};
    float limit21 = blockSize.y() / 2;
    QVector3D startPoint22 = {
            -blockSize.x() / 2,
            blockSize.z() - BottomDepth,
            5.6};
    float limit22 = -blockSize.y() / 2;
    auto roughtFlat1 = GenerateFlatZigZag(confMapF12, startPoint21, limit21 , F12_RADIUS - 0.05f, blockSize,
                                         texSize, Y, false);
    auto roughtFlat2 = GenerateFlatZigZag(confMapF12, startPoint22, limit22, F12_RADIUS - 0.05f, blockSize,
                                          texSize, Y, false);
    CutterPath flatPath(CutterParameters(Length::FromSceneUnits(F12_RADIUS * 2), CutterType::Cylindrical));
    flatPath.Points.insert(flatPath.Points.end(), roughtFlat1.begin(), roughtFlat1.end());
    flatPath.Points.insert(flatPath.Points.end(), roughtFlat2.begin(), roughtFlat2.end());
    GCodeSaver::SaveCutterPath(folderName, flatPath, 2);

    //5. Generowanie obróbki dokładnej dla płaskiej podstawki
    std::shared_ptr<RoutingAware> Body, Ucho, Pokrywka, Dziubek;
    PlaneCreator planeCreator("routingPlaneCreator", PLANEC2_CLASS, {-7.5f, 0.0f, -7.5f});
    auto elements = GetComponents();
    Body = elements[0].lock();
    Ucho = elements[1].lock();
    Dziubek = elements[2].lock();
    Pokrywka = elements[3].lock();

    planeCreator.p_UVParams->U = 10;
    planeCreator.p_UVParams->V = 10;
    planeCreator.p_UVParams->Width = 10.2;
    planeCreator.p_UVParams->Height = 15;
    planeCreator.p_UVParams->IsPipe = false;
    auto stol = planeCreator.p_UVParams->CreatePlane<PlaneC2>("stol");
    planeCreator.p_Transform->Position = QVector3D(3.0, 0, -7.5);
    planeCreator.p_UVParams->Width = 4.8;
    planeCreator.p_UVParams->U = 6;
    auto stol2 = planeCreator.p_UVParams->CreatePlane<PlaneC2>("stol2");

    OffsetPlane BodyOffset(Body->p_Intersection, F10_RADIUS);
    OffsetPlane UchoOffset(Ucho->p_Intersection, F10_RADIUS);
    OffsetPlane DziubekOffset(Dziubek->p_Intersection, F10_RADIUS);
    OffsetPlane PokrywkaOffset(Pokrywka->p_Intersection, F10_RADIUS);

    std::shared_ptr<IntersectionAwareSystem> isys;
    if (auto scene = SceneECS::Instance().lock())
        if (auto is = scene->GetSystem<IntersectionAwareSystem>().lock())
            isys = is;

    //Przeciecia stol-body
    QVector3D StolBodyStart1 = {-4.7, -0.3, 5.4}; //prawa strona
    auto StolBodyCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, BodyOffset.p_Intersection, StolBodyStart1);
    auto precFlat4 = StolBodyCurve1->p_IntersectionRes->GetScenePoints();
    QVector3D StolBodyStart2 = {-4.7, -0.3, -5.4}; //lewa strona
    auto StolBodyCurve2 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, BodyOffset.p_Intersection, StolBodyStart2);
    auto precFlat5 = StolBodyCurve2->p_IntersectionRes->GetScenePoints();

    //Przecięcia stol-ucho
    QVector3D StolUchoStart1 = {-3.2, -0.4, 4.3};
    auto StolUchoCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, UchoOffset.p_Intersection, StolUchoStart1);
    auto precFlat9 = StolUchoCurve1->p_IntersectionRes->GetScenePoints();

    //Przeciecia stol-dziubek
    QVector3D StolDziubekStart1 = {0.8, -0.4, -3.7};//gora
    auto StolDziubekCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, DziubekOffset.p_Intersection, StolDziubekStart1);
    auto precFlat7 = StolDziubekCurve1->p_IntersectionRes->GetScenePoints();
    QVector3D StolDziubekStart2 =   {-3.2, -0.4, -3.7};//dol
    auto StolDziubekCurve2 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, DziubekOffset.p_Intersection, StolDziubekStart2);
    auto precFlat8 = StolDziubekCurve2->p_IntersectionRes->GetScenePoints();

    //Przeciecia stol-pokrywka
    QVector3D StolPokrywkaStart1 = {3.1, -0.3, -2.1};//gora lewa
    QVector3D StolPokrywkaStart2 = {3.1, 0.3, 1.8};//gora prawa
    QVector3D StolPokrywkaStart3 = {2.1, -0.2, 1.8};//dol prawa
    QVector3D StolPokrywkaStart4 = {1.8, 1.0, -1.5};//dol lewa
    auto StolPokrywkaCurve1 = isys->CreateIntersectionCurveBetween(stol2->p_Intersection, PokrywkaOffset.p_Intersection, StolPokrywkaStart1);
    auto StolPokrywkaCurve2 = isys->CreateIntersectionCurveBetween(stol2->p_Intersection, PokrywkaOffset.p_Intersection, StolPokrywkaStart2);
    auto StolPokrywkaCurve3 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, PokrywkaOffset.p_Intersection, StolPokrywkaStart3);
    auto StolPokrywkaCurve4 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, PokrywkaOffset.p_Intersection, StolPokrywkaStart4);
    auto precFlat1 = StolPokrywkaCurve1->p_IntersectionRes->GetScenePoints();
    auto precFlat2 = StolPokrywkaCurve2->p_IntersectionRes->GetScenePoints();
    auto precFlat3 = StolPokrywkaCurve3->p_IntersectionRes->GetScenePoints();
    auto precFlat6 = StolPokrywkaCurve4->p_IntersectionRes->GetScenePoints();

    PlaneDivision planeDiv;
    planeDiv.AddConstraintPolyline(precFlat1);//
    planeDiv.AddConstraintPolyline(precFlat2);
    planeDiv.AddConstraintPolyline(precFlat3);
    planeDiv.AddConstraintPolyline(precFlat4);
    planeDiv.AddConstraintPolyline(precFlat5);
    planeDiv.AddConstraintPolyline(precFlat6);
    planeDiv.AddConstraintPolyline(precFlat7);//gora dziubek
    planeDiv.AddConstraintPolyline(precFlat8);
    planeDiv.AddConstraintPolyline(precFlat9);

    QVector4D planeSize = QVector4D(
            blockWorldPos.x() - blockSize.x() / 2,
            blockWorldPos.z() - blockSize.y() / 2,
            blockWorldPos.x() + blockSize.x() / 2,
            blockWorldPos.z() + blockSize.y() / 2
            );
    planeDiv.CreateDivision(planeSize);

    /*CutterPath flatPrecisionPath(CutterParameters(Length::FromSceneUnits(F10_RADIUS * 2), CutterType::Cylindrical));
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat1.begin(), precFlat1.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat2.begin(), precFlat2.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat3.begin(), precFlat3.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat4.begin(), precFlat4.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat5.begin(), precFlat5.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat6.begin(), precFlat6.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat7.begin(), precFlat7.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat8.begin(), precFlat8.end());
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), precFlat9.begin(), precFlat9.end());

    for (QVector3D& p : flatPrecisionPath.Points)
        p = FromSceneToBlock(p, blockWorldPos);
    GCodeSaver::SaveCutterPath(folderName, flatPrecisionPath, 3);*/

    // Wyczyszczenie zasobów
    zmapTex->destroy();
    gl->doneCurrent();
    stol->RemovePointsInside();
    stol2->RemovePointsInside();
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
                                        QVector3D blockSize, QSize texSize, float tolerance, ZigZagVariable variable,
                                        ZigZagDirection direction)
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
        int idx = texPoint.y() * texSize.width() + texPoint.x();
        if (idx >= 0 && idx < confMap.size())//co najwyzej dwukrotnosc tolerancji bledu na powierzchni
            height = std::max(height, confMap[idx] + 2 * tolerance);

        zigzag[i].first = texPoint;
        zigzag[i].second = height;
    }

    //Optymalizacja sciezki - wyrzucenie prostych odcinkow oraz zdyskretyzowanie zbocz
    return OptimizeRouterPath(zigzag, tolerance, startPoint.y(), blockSize, texSize);
}

std::vector<QVector3D>
RoutingAwareSystem::GenerateFlatZigZag(const std::vector<float> &confMap, QVector3D startPoint, float targetWidth,
                                       float w, QVector3D blockSize, QSize texSize,
                                       RoutingAwareSystem::ZigZagVariable variable, bool flipZigZag = false)
{
    //Obróbka przesuwajac sie po X w strone ujemna
    QPoint texStartPoint = FromBlockToTex(QVector2D(startPoint.x(), startPoint.z()), texSize, blockSize);
    int texW = w / blockSize.x() * texSize.width();
    int texTargetWidth = FromBlockToTex(QVector2D(targetWidth, 0), texSize, blockSize).x();

    //Wygenerowanie zygzaka bez uwzglednienia dopuszczalnej wysokosci
    QPoint bounduaries = {-2 * texW, texSize.width() + 2 * texW};
    auto zigzag = CreateZigZagLines(texStartPoint, texW, bounduaries, texTargetWidth, variable);

    //Znalezienie koncow linii bez kolizji z obiektem
    std::vector<std::pair<QVector3D, QVector3D>> lineEnds;
    std::vector<bool> lineCuts;
    for (const auto &line: zigzag)
    {
        auto blockPosStart = FromTexToBlock(line.front(), texSize, blockSize);
        auto blockPosEnd = FromTexToBlock(line.back(), texSize, blockSize);
        QVector3D start = {blockPosStart.x(), startPoint.y(), blockPosStart.y()};
        QVector3D end = {blockPosEnd.x(), startPoint.y(), blockPosEnd.y()};
        bool cuts = false;
        int i;
        for (const QPoint &p: line)
        {
            if (p.x() < 0 || p.x() >= texSize.width() || p.y() < 0 || p.y() >= texSize.height())
                continue;

            int idx = p.y() * texSize.width() + p.x();
            if (confMap[idx] > startPoint.y())
            {
                auto blockPos = FromTexToBlock(p, texSize, blockSize);
                end = {blockPos.x(), startPoint.y(), blockPos.y()};
                cuts = true;
                break;
            }
        }

        lineEnds.emplace_back(std::make_pair(start, end));
        lineCuts.emplace_back(cuts);
    }

    std::vector<QVector3D> path;
    bool swapLine = flipZigZag;
    for (int i = 0; i < lineEnds.size(); ++i)
    {
        auto line = lineEnds[i];

        if (swapLine)
        {
            path.emplace_back(line.second);
            path.emplace_back(line.first);
        } else
        {
            path.emplace_back(line.first);
            path.emplace_back(line.second);
        }
        swapLine = !swapLine;
    }

    return path;
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
        for (int x = startPoint.x();
             direction == Positive ? x < planeSize.width() : x >= 0; x += width * (direction == Positive ? 1 : -1))
        {
            for (int y = negInner ? planeSize.height() - 1 : 0;
                 negInner ? y >= 0 : y <= planeSize.height() - 1; y += (negInner ? -1 : 1))
            {
                zigzag.emplace_back(std::make_pair(QPoint(x, y), height));
            }

            if (direction == Positive ? x + width <= planeSize.width() : x - width >= 0)
            {
                for (int i = x + (direction == Positive ? 1 : -1);
                     direction == Positive ? i < x + width : i > x - width; i += (direction == Positive ? 1 : -1))
                    zigzag.emplace_back(std::make_pair(QPoint(i, negInner ? 0 : planeSize.height() - 1), height));
            }
            negInner = !negInner;
        }
    } else
    {
        bool negInner = startPoint.x() > (planeSize.width() / 2);
        for (int y = startPoint.y();
             direction == Positive ? y < planeSize.height() : y >= 0; y += width * (direction == Positive ? 1 : -1))
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

std::vector<std::pair<QPoint, float>>
RoutingAwareSystem::CreateZigZagPoints(QPoint startPoint, int width, float height, QPoint zigZagBoundaries,
                                       int targetWidth, RoutingAwareSystem::ZigZagVariable variable)
{
    std::vector<std::pair<QPoint, float>> zigzag;

    if (variable == X)
    {
        int incrementWidth = width * (targetWidth - startPoint.x() > 0 ? 1 : -1);
        bool negInner = (startPoint.y() > (zigZagBoundaries.y() - zigZagBoundaries.x()) / 2);
        for (int x = startPoint.x(); incrementWidth > 0 ? x < targetWidth : x > targetWidth; x += incrementWidth)
        {
            for (int y = negInner ? zigZagBoundaries.y() : zigZagBoundaries.x();
                 negInner ? y >= zigZagBoundaries.x() : y <= zigZagBoundaries.y(); y += (negInner ? -1 : 1))
            {
                zigzag.emplace_back(std::make_pair(QPoint(x, y), height));
            }

            if (incrementWidth > 0 ? x + incrementWidth <= targetWidth : x + incrementWidth >= targetWidth)
            {
                for (int i = x + (incrementWidth > 0 ? 1 : -1);
                     incrementWidth > 0 ? i <= x + incrementWidth : i >= x + incrementWidth;
                     i += (incrementWidth > 0 ? 1 : -1))
                    zigzag.emplace_back(
                            std::make_pair(QPoint(i, negInner ? zigZagBoundaries.x() : zigZagBoundaries.y()),
                                           height));
            }
            negInner = !negInner;
        }
    } else
    {
        int incrementWidth = width * (targetWidth - startPoint.y() > 0 ? 1 : -1);
        bool negInner = (startPoint.x() > (zigZagBoundaries.y() - zigZagBoundaries.x()) / 2);
        for (int y = startPoint.y(); incrementWidth > 0 ? y < targetWidth : y > targetWidth; y += incrementWidth)
        {
            for (int x = negInner ? zigZagBoundaries.y() : zigZagBoundaries.x();
                 negInner ? x >= 0 : x <= zigZagBoundaries.y(); x += negInner ? -1 : 1)
                zigzag.emplace_back(std::make_pair(QPoint(x, y), height));

            if (incrementWidth > 0 ? y + incrementWidth <= targetWidth : y + incrementWidth >= targetWidth)
            {
                for (int i = y + (incrementWidth > 0 ? 1 : -1);
                     incrementWidth > 0 ? i <= y + incrementWidth : i >= y + incrementWidth; i += (incrementWidth > 0
                                                                                                   ? 1 : -1))
                    zigzag.emplace_back(
                            std::make_pair(QPoint(negInner ? zigZagBoundaries.x() : zigZagBoundaries.y(), i), height));
            }
            negInner = !negInner;
        }
    }

    return zigzag;
}

std::vector<std::vector<QPoint>>
RoutingAwareSystem::CreateZigZagLines(QPoint startPoint, int width, QPoint zigZagBoundaries, int targetWidth,
                                      RoutingAwareSystem::ZigZagVariable variable)
{
    std::vector<std::vector<QPoint>> zigzags;

    if (variable == X)
    {
        int incrementWidth = width * (targetWidth - startPoint.x() > 0 ? 1 : -1);
        bool negInner = (startPoint.y() > (zigZagBoundaries.y() - zigZagBoundaries.x()) / 2);
        for (int x = startPoint.x(); incrementWidth > 0 ? x < targetWidth : x > targetWidth; x += incrementWidth)
        {
            std::vector<QPoint> zigzag;
            for (int y = negInner ? zigZagBoundaries.y() : zigZagBoundaries.x();
                 negInner ? y >= zigZagBoundaries.x() : y <= zigZagBoundaries.y(); y += (negInner ? -1 : 1))
            {
                zigzag.emplace_back(QPoint(x, y));
            }

            zigzags.emplace_back(zigzag);
        }
    } else
    {
        int incrementWidth = width * (targetWidth - startPoint.y() > 0 ? 1 : -1);
        bool negInner = (startPoint.x() > (zigZagBoundaries.y() - zigZagBoundaries.x()) / 2);
        for (int y = startPoint.y(); incrementWidth > 0 ? y < targetWidth : y > targetWidth; y += incrementWidth)
        {
            std::vector<QPoint> zigzag;
            for (int x = negInner ? zigZagBoundaries.y() : zigZagBoundaries.x();
                 negInner ? x >= zigZagBoundaries.x() : x <= zigZagBoundaries.y(); x += negInner ? -1 : 1)
                zigzag.emplace_back(QPoint(x, y));

            zigzags.emplace_back(zigzag);
        }
    }

    return zigzags;
}

std::vector<QVector3D>
RoutingAwareSystem::OptimizeRouterPath(const std::vector<std::pair<QPoint, float>> &zigzag, float tolerance,
                                       float maxDepth, QVector3D blockSize, QSize texSize)
{
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
        else if (p.second == maxDepth && std::abs(p.second - optimisedPath.back().y()) > 0)
        {
            auto blockPos = FromTexToBlock(p.first, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), maxDepth, blockPos.y());
        }
            //Przypadek zmiany kierunku
        else if (lastDir != dir)
        {
            auto blockPos = FromTexToBlock(lastP, texSize, blockSize);
            optimisedPath.emplace_back(blockPos.x(), maxDepth, blockPos.y());
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
    optimisedPath.emplace_back(blockPos.x(), maxDepth, blockPos.y());

    return optimisedPath;
}

QVector3D RoutingAwareSystem::FromSceneToBlock(QVector3D sceneP, QVector3D blockWorldPos)
{
    return sceneP - blockWorldPos;
}




