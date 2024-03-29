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
    roughPath.Points = AddSafeStartEndPosition(roughPath.Points, {0.0f, 7.0f, 0.0f});
    GCodeSaver::SaveCutterPath(folderName, roughPath, 1);

    // 4.Generowanie obrobki zgrubnej dla plaskiej podstawki
    QVector3D startPoint21 = {
            blockSize.x() / 2,
            blockSize.z() - BottomDepth,
            -blockSize.y() / 2 - 1.2f * F12_RADIUS};
    float limit21 = blockSize.y() / 2 + F12_RADIUS;
    QVector3D startPoint22 = {
            -blockSize.x() / 2,
            blockSize.z() - BottomDepth,
            5.6};
    float limit22 = -blockSize.y() / 2;
    auto roughtFlat1 = GenerateFlatZigZag(confMapF12, startPoint21, limit21, F12_RADIUS - 0.05f, blockSize,
                                          texSize, Y, false);
    auto roughtFlat2 = GenerateFlatZigZag(confMapF12, startPoint22, limit22, F12_RADIUS - 0.05f, blockSize,
                                          texSize, Y, false);
    CutterPath flatPath(CutterParameters(Length::FromSceneUnits(F12_RADIUS * 2), CutterType::Cylindrical));
    flatPath.Points.insert(flatPath.Points.end(), roughtFlat1.begin() + 1, roughtFlat1.end());
    flatPath.Points.insert(flatPath.Points.end(), roughtFlat2.begin(), roughtFlat2.end());
    flatPath.Points = AddSafeStartEndPosition(flatPath.Points, {0.0f, 5.0f, 0.0f});
    GCodeSaver::SaveCutterPath(folderName, flatPath, 2);

    //5. Generowanie obróbki dokładnej dla płaskiej podstawki

    auto offsetPoints = GenerateFlatPrecisionPath(blockWorldPos, blockSize);

    CutterPath flatPrecisionPath(CutterParameters(Length::FromSceneUnits(F10_RADIUS * 2), CutterType::Cylindrical));
    flatPrecisionPath.Points.emplace_back(QVector3D(blockSize.x() / 2.0f, 0.0f, offsetPoints.front().z()) + QVector3D(2.0f * F10_RADIUS, 0.0f, 0.0f));
    flatPrecisionPath.Points.insert(flatPrecisionPath.Points.end(), offsetPoints.begin(), offsetPoints.end());
    flatPrecisionPath.Points = AddSafeStartEndPosition(flatPrecisionPath.Points, {0.0f, 2.5f, 0.0f});
    for (QVector3D &p: flatPrecisionPath.Points)
        p = FromSceneToBlock(p, blockWorldPos);
    GCodeSaver::SaveCutterPath(folderName, flatPrecisionPath, 3);

    //Etap 3 - obróbka dokładna

#pragma region Generowanie przeciec
    std::shared_ptr<RoutingAware> Body, Ucho, Pokrywka, Dziubek;
    auto elements = GetComponents();
    Body = elements[0].lock();
    Ucho = elements[1].lock();
    Dziubek = elements[2].lock();
    Pokrywka = elements[3].lock();

    std::shared_ptr<IntersectionAwareSystem> isys;
    if (auto scene = SceneECS::Instance().lock())
        if (auto is = scene->GetSystem<IntersectionAwareSystem>().lock())
            isys = is;

    PlaneCreator planeCreator("routingPlaneCreator", PLANEC2_CLASS, {-7.5f, 0.0f, -7.5f});
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

    OffsetPlane BodyOffsetK8(Body->p_Intersection, K8_RADIUS);
    OffsetPlane UchoOffsetK8(Ucho->p_Intersection, K8_RADIUS);
    OffsetPlane DziubekOffsetK8(Dziubek->p_Intersection, K8_RADIUS);
    OffsetPlane PokrywkaOffsetK8(Pokrywka->p_Intersection, K8_RADIUS);
    OffsetPlane StolOffsetK8(stol->p_Intersection, -K8_RADIUS);
    OffsetPlane Stol2OffsetK8(stol2->p_Intersection, -K8_RADIUS);

    //Przeciecia stol-body
    QVector3D StolBodyStart = {-4.7, -0.3, 5.4};
    auto StolBodyCurve = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection, BodyOffsetK8.p_Intersection,
                                                              StolBodyStart, 0.2f);

    //Przecięcia stol-ucho
    QVector3D StolUchoStart1 = {-3.2, -0.4, 4.3};
    auto StolUchoCurve1 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection, UchoOffsetK8.p_Intersection,
                                                               StolUchoStart1, 0.1f);
    QVector3D StolUchoStart2 = {-0.7, -0.6, 3.9};
    auto StolUchoCurve2 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection, UchoOffsetK8.p_Intersection,
                                                               StolUchoStart2, 0.1f);

    //Przeciecia stol-dziubek
    QVector3D StolDziubekStart1 = {0.8, -0.4, -3.7};//gora
    auto StolDziubekCurve1 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection,
                                                                  DziubekOffsetK8.p_Intersection, StolDziubekStart1);
    QVector3D StolDziubekStart2 = {-3.2, -0.4, -3.7};//dol
    auto StolDziubekCurve2 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection,
                                                                  DziubekOffsetK8.p_Intersection, StolDziubekStart2);

    //Przeciecia stol-pokrywka
    QVector3D StolPokrywkaStart1 = {3.1, -0.3, -2.1};//gora lewa
    QVector3D StolPokrywkaStart2 = {3.1, 0.3, 1.8};//gora prawa
    QVector3D StolPokrywkaStart3 = {2.1, -0.2, 1.8};//dol prawa
    QVector3D StolPokrywkaStart4 = {1.8, 1.0, -1.5};//dol lewa
    auto StolPokrywkaCurve1 = isys->CreateIntersectionCurveBetween(Stol2OffsetK8.p_Intersection,
                                                                   PokrywkaOffsetK8.p_Intersection, StolPokrywkaStart1);
    //auto StolPokrywkaCurve2 = isys->CreateIntersectionCurveBetween(Stol2OffsetK8.p_Intersection,
    //                                                               PokrywkaOffsetK8.p_Intersection, StolPokrywkaStart2);
    auto StolPokrywkaCurve3 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection,
                                                                   PokrywkaOffsetK8.p_Intersection, StolPokrywkaStart3);
    auto StolPokrywkaCurve4 = isys->CreateIntersectionCurveBetween(StolOffsetK8.p_Intersection,
                                                                   PokrywkaOffsetK8.p_Intersection, StolPokrywkaStart4);

    //Przeciecia dziubek-body
    QVector3D DziubekBodyStart = {-1.4, 0.9, -2.5};
    auto DziubekBodyCurve = isys->CreateIntersectionCurveBetween(DziubekOffsetK8.p_Intersection,
                                                                 BodyOffsetK8.p_Intersection, DziubekBodyStart);

    //Przeciecia ucho-body
    QVector3D UchoBodyStart1 = {0.4, 0.3, 2.6};
    auto UchoBodyCurve1 = isys->CreateIntersectionCurveBetween(UchoOffsetK8.p_Intersection, BodyOffsetK8.p_Intersection,
                                                               UchoBodyStart1);
    QVector3D UchoBodyStart2 = {-2.7, 0.3, 3.7};
    auto UchoBodyCurve2 = isys->CreateIntersectionCurveBetween(UchoOffsetK8.p_Intersection, BodyOffsetK8.p_Intersection,
                                                               UchoBodyStart2);

    //Przeciecie pokrywka-body
    QVector3D PokrywkaBodyStart = {1.5, 1.2, -0.6};
    auto PokrywkaBodyCurve = isys->CreateIntersectionCurveBetween(PokrywkaOffsetK8.p_Intersection,
                                                                  BodyOffsetK8.p_Intersection, PokrywkaBodyStart);

#pragma endregion
    //Ograniczenia do obrobki Dziubka
#pragma region Precyzyjny dziubek
    PlaneDivision divDziubek(QVector4D(0, 0, DziubekOffsetK8.p_UV->U, DziubekOffsetK8.p_UV->V));
    divDziubek.WrapX = true;

    divDziubek.AddConstraintPolyline(DziubekBodyCurve->p_IntersectionRes->GetFirstParameterPoints(), true);
    divDziubek.AddConstraintPolyline(StolDziubekCurve1->p_IntersectionRes->GetSecondParameterPoints());
    divDziubek.AddConstraintPolyline(StolDziubekCurve2->p_IntersectionRes->GetSecondParameterPoints());

    float DziubekDu = 0.1f;
    float DziubekDv = 0.1f;
    AddLinesAsConstrains(QVector2D(DziubekOffsetK8.p_UV->U, DziubekOffsetK8.p_UV->V - 0.01f), DziubekDv,
                         DziubekDu / 2.0f,
                         DziubekOffsetK8.p_UV->V,
                         QVector2D(0.0f, DziubekOffsetK8.p_UV->U + 0.01f), Y, divDziubek);
    QVector2D DziubekStartPoint1 = StolDziubekCurve1->p_IntersectionRes->GetSecondParameterPoints().back();
    AddLinesAsConstrains(DziubekStartPoint1, DziubekDu, DziubekDv,
                         0.0f,
                         QVector2D(0.0f, DziubekOffsetK8.p_UV->V), X, divDziubek);
    QVector2D DziubekStartPoint2 = StolDziubekCurve2->p_IntersectionRes->GetSecondParameterPoints().front();
    AddLinesAsConstrains(DziubekStartPoint2, DziubekDu, DziubekDv,
                         DziubekOffsetK8.p_UV->U,
                         QVector2D(0.0f, DziubekOffsetK8.p_UV->V), X, divDziubek);

    divDziubek.CreateDivision();

    int skipFront = 5;
    int skipBack = 1;
    std::vector<int> linesToVisit(20 - skipFront);
    std::iota(std::begin(linesToVisit), std::end(linesToVisit), skipFront);
    linesToVisit.emplace_back(22);
    linesToVisit.emplace_back(23);
    linesToVisit.emplace_back(24);
    linesToVisit.emplace_back(25);
    linesToVisit.emplace_back(26);
    linesToVisit.emplace_back(27);

    //divDziubek.DebugImages = true;
    auto dziubekParameterPath = divDziubek.JoinConstraintPolylinesZigzag(linesToVisit, {0, 1, 2, 3}, true, true,
                                                                         22, 8);

    std::vector<QVector3D> dziubekPrecPath = FromParams(dziubekParameterPath, DziubekOffsetK8.p_Intersection,
                                                        K8_RADIUS);
#pragma endregion

#pragma region Precyzyjne ucho
    //Ograniczenia do obrobki ucha
    PlaneDivision divUcho(QVector4D(0, 0, UchoOffsetK8.p_UV->U, UchoOffsetK8.p_UV->V));
    divUcho.WrapX = true;

    divUcho.AddConstraintPolyline(UchoBodyCurve1->p_IntersectionRes->GetFirstParameterPoints(), true);
    divUcho.AddConstraintPolyline(UchoBodyCurve2->p_IntersectionRes->GetFirstParameterPoints(), true);
    divUcho.AddConstraintPolyline(StolUchoCurve1->p_IntersectionRes->GetSecondParameterPoints());
    divUcho.AddConstraintPolyline(StolUchoCurve2->p_IntersectionRes->GetSecondParameterPoints());

    float UchoDu = 0.1f;
    float UchoDv = 0.1f;
    QVector2D UchoStartPoint1 = StolUchoCurve1->p_IntersectionRes->GetSecondParameterPoints().back();
    AddLinesAsConstrains(UchoStartPoint1, UchoDu, UchoDv, 0.0f,
                         QVector2D(0.0f, UchoOffsetK8.p_UV->V), X, divUcho);
    QVector2D UchoStartPoint2 = StolUchoCurve2->p_IntersectionRes->GetSecondParameterPoints().front();
    AddLinesAsConstrains(UchoStartPoint2, UchoDu, UchoDv, UchoOffsetK8.p_UV->U,
                         QVector2D(0.0f, UchoOffsetK8.p_UV->V), X, divUcho);

    divUcho.CreateDivision();

    int uchoSkipFront = 5;
    std::vector<int> uchoLinesToVisit(19 - uchoSkipFront);
    std::iota(std::begin(uchoLinesToVisit), std::end(uchoLinesToVisit), uchoSkipFront);
    uchoLinesToVisit.emplace_back(20);
    uchoLinesToVisit.emplace_back(21);
    uchoLinesToVisit.emplace_back(22);
    uchoLinesToVisit.emplace_back(23);
    //divUcho.DebugImages = true;
    auto uchoParameterPath = divUcho.JoinConstraintPolylinesZigzag(uchoLinesToVisit, {0, 1, 2, 3}, true, true,
                                                                   20, 18);

    std::vector<QVector3D> uchoPrecPath = FromParams(uchoParameterPath, UchoOffsetK8.p_Intersection, K8_RADIUS);
#pragma endregion

#pragma region Precyzyjne body
    //Ograniczenia do obrobki body
    PlaneDivision divBody(QVector4D(0, 0, BodyOffsetK8.p_UV->U, BodyOffsetK8.p_UV->V));
    divBody.WrapX = true;

    divBody.AddConstraintPolyline(StolBodyCurve->p_IntersectionRes->GetSecondParameterPoints());
    divBody.AddConstraintPolyline(PokrywkaBodyCurve->p_IntersectionRes->GetSecondParameterPoints(), true);
    divBody.AddConstraintPolyline(UchoBodyCurve1->p_IntersectionRes->GetSecondParameterPoints(), true);
    divBody.AddConstraintPolyline(UchoBodyCurve2->p_IntersectionRes->GetSecondParameterPoints(), true);
    divBody.AddConstraintPolyline(DziubekBodyCurve->p_IntersectionRes->GetSecondParameterPoints(), true);

    //divBody.DebugImages = true;
    divBody.CreateDivision();

    auto swype2Params = divBody.JoinConstraintPolylinesTogetherInCycle(true, false, 1);
    auto swype2PrecPath = FromParams(swype2Params, BodyOffsetK8.p_Intersection, K8_RADIUS);



    float BodyDu = 0.05f;
    float BodyDv = 0.1f;
    QVector2D BodyStartPoint1 =
            UchoBodyCurve2->p_IntersectionRes->GetSecondParameterPoints().front() - QVector2D(0.5f * BodyDu, 0.0f);
    AddLinesAsConstrains(BodyStartPoint1, BodyDu, BodyDv, 0.0f,
                         QVector2D(0.0f, BodyOffsetK8.p_UV->V), X, divBody);
    QVector2D BodyStartPoint2 =
            DziubekBodyCurve->p_IntersectionRes->GetSecondParameterPoints().front() - QVector2D(0.28f, 0.0f);
    int secondCount = AddLinesAsConstrains(BodyStartPoint2, BodyDu, BodyDv, BodyOffsetK8.p_UV->U,
                                           QVector2D(0.0f, BodyOffsetK8.p_UV->V), X, divBody);

    //divBody.DebugImages = true;
    divBody.DebugImages = false;
    divBody.CreateDivision();

    int bodySkipFront = 5;
    std::vector<int> bodyLinesToVisit(divBody.GetConstraintCount() - bodySkipFront);
    std::iota(std::begin(bodyLinesToVisit), std::end(bodyLinesToVisit), bodySkipFront);

    //divBody.DebugImages = true;
    auto bodyParameterPath1 = divBody.JoinConstraintPolylinesZigzag(bodyLinesToVisit, {0, 1, 2, 3, 4}, true, true,
                                                                    divBody.GetConstraintCount() - secondCount, 20);

    std::vector<int> bodyLinesToVisit2(10);
    std::iota(std::begin(bodyLinesToVisit2), std::end(bodyLinesToVisit2), divBody.GetConstraintCount() - secondCount);
    //divBody.DebugImages = true;
    auto bodyParameterPath2 = divBody.JoinConstraintPolylinesZigzag(bodyLinesToVisit2, {0, 1, 2, 3, 4}, true, true,
                                                                    divBody.GetConstraintCount() - secondCount, 36);

    AddLinesAsConstrains(QVector2D(BodyOffsetK8.p_UV->U, 2.8f), BodyDv, BodyDu,
                         2.8f + BodyDv / 2.0f,
                         QVector2D(0.0f, BodyOffsetK8.p_UV->U), Y, divBody);
    //divBody.DebugImages = true;
    divBody.CreateDivision();

    std::vector<int> bodyLinesToVisit3(7);
    std::iota(std::begin(bodyLinesToVisit3), std::end(bodyLinesToVisit3), 5);
    auto bodyParameterPath3 = divBody.JoinConstraintPolylinesZigzag(bodyLinesToVisit3,
                                                                    {0, 1, 2, 3, 4, divBody.GetConstraintCount() - 1},
                                                                    true, true, 6, 44);

    std::vector<QVector3D> bodyPrecPath1 = FromParams(bodyParameterPath1, BodyOffsetK8.p_Intersection, K8_RADIUS);
    std::vector<QVector3D> bodyPrecPath2 = FromParams(bodyParameterPath2, BodyOffsetK8.p_Intersection, K8_RADIUS);
    std::vector<QVector3D> bodyPrecPath3 = FromParams(bodyParameterPath3, BodyOffsetK8.p_Intersection, K8_RADIUS);
#pragma endregion

#pragma region Precyzyjna pokrywka
    //Ograniczenia do obrobki Pkrywki
    PlaneDivision divPokrywka(QVector4D(0, 0, PokrywkaOffsetK8.p_UV->U, PokrywkaOffsetK8.p_UV->V));
    divPokrywka.WrapX = true;

    divPokrywka.AddConstraintPolyline(StolPokrywkaCurve1->p_IntersectionRes->GetSecondParameterPoints());
    //divPokrywka.AddConstraintPolyline(StolPokrywkaCurve2->p_IntersectionRes->GetSecondParameterPoints());
    divPokrywka.AddConstraintPolyline(StolPokrywkaCurve3->p_IntersectionRes->GetSecondParameterPoints());
    divPokrywka.AddConstraintPolyline(StolPokrywkaCurve4->p_IntersectionRes->GetSecondParameterPoints());
    divPokrywka.AddConstraintPolyline(PokrywkaBodyCurve->p_IntersectionRes->GetFirstParameterPoints(), true);

    const float PokrywkaDu = 0.05f;
    const float PokrywkaDv = 0.1f;
    const float VConstraint1 = 2.0f;
    const float VConstraint2 = 4.0f;
    AddLinesAsConstrains(QVector2D(PokrywkaOffsetK8.p_UV->U, VConstraint1), PokrywkaDv, PokrywkaDu,
                         VConstraint1 + PokrywkaDv / 2.0f,
                         QVector2D(0.0f, PokrywkaOffsetK8.p_UV->U), Y, divPokrywka, true);
    AddLinesAsConstrains(QVector2D(PokrywkaOffsetK8.p_UV->U, VConstraint2), PokrywkaDv, PokrywkaDu,
                         VConstraint2 + PokrywkaDv / 2.0f,
                         QVector2D(0.0f, PokrywkaOffsetK8.p_UV->U), Y, divPokrywka, true);
    QVector2D PokrywkaStartPoint1 = StolPokrywkaCurve1->p_IntersectionRes->GetSecondParameterPoints().back();
    AddLinesAsConstrains(PokrywkaStartPoint1, PokrywkaDu, PokrywkaDv,
                         0.0f,
                         QVector2D(0.0f, PokrywkaOffsetK8.p_UV->V), X, divPokrywka);
    QVector2D PokrywkaStartPoint2 =
            StolPokrywkaCurve1->p_IntersectionRes->GetSecondParameterPoints().front() + QVector2D(PokrywkaDu, 0.0f);
    int pokrywkaSecondCount = AddLinesAsConstrains(PokrywkaStartPoint2, PokrywkaDu, PokrywkaDv,
                                                   PokrywkaOffsetK8.p_UV->U,
                                                   QVector2D(0.0f, PokrywkaOffsetK8.p_UV->V), X, divPokrywka);


    //divPokrywka.DebugImages = true;
    divPokrywka.CreateDivision();
    //divPokrywka.DebugImages = false;

    int pokrywkaSkipFront = 7;
    std::vector<int> pokrywkaLinesToVisit(divPokrywka.GetConstraintCount() - pokrywkaSkipFront);
    std::iota(std::begin(pokrywkaLinesToVisit), std::end(pokrywkaLinesToVisit), pokrywkaSkipFront);

    auto pokrywkaParameterPath1 = divPokrywka.JoinConstraintPolylinesZigzag(pokrywkaLinesToVisit, {0, 1, 2, 3, 4, 5},
                                                                            true, true,
                                                                            divPokrywka.GetConstraintCount() -
                                                                            pokrywkaSecondCount, 10);
    //divPokrywka.DebugImages = true;
    auto pokrywkaParameterPath2 = divPokrywka.JoinConstraintPolylinesZigzag(pokrywkaLinesToVisit, {0, 1, 2, 3, 4, 5},
                                                                            true, true,
                                                                            divPokrywka.GetConstraintCount() -
                                                                            pokrywkaSecondCount, 45);
    auto pokrywkaPrecPath1 = FromParams(pokrywkaParameterPath1, PokrywkaOffsetK8.p_Intersection, K8_RADIUS);
    auto pokrywkaPrecPath2 = FromParams(pokrywkaParameterPath2, PokrywkaOffsetK8.p_Intersection, K8_RADIUS);
    std::vector<QVector3D> pokrywkaPrecPath3;
    const float v = 3.0f;
    for (float u = PokrywkaStartPoint2.x(); u < PokrywkaStartPoint1.x() + PokrywkaOffsetK8.p_UV->U; u += PokrywkaDu)
    {
        //Policzenie normalki w szczegolnym przypadku
        QVector3D derU = PokrywkaOffsetK8.p_Intersection->SceneFunctionDerU({u, v}).normalized();
        QVector3D derV = {1.0f, 0.0f, 0.0f};
        QVector3D norm = QVector3D::crossProduct(derU, derV).normalized();
        pokrywkaPrecPath3.emplace_back(Pokrywka->p_Intersection->SceneFunction({u, v}) + (K8_RADIUS + 0.01f) * norm -
                                       QVector3D(0.0f, K8_RADIUS, 0.0f));
    }
#pragma endregion

#pragma region Precyzyjna dziura
    //Ograniczenia do obrobki Dziury
    PlaneDivision divDziura(QVector4D(0, StolOffsetK8.p_UV->V / 2.0f, StolOffsetK8.p_UV->U, StolOffsetK8.p_UV->V));

    divDziura.AddConstraintPolyline(StolBodyCurve->p_IntersectionRes->GetFirstParameterPoints());
    divDziura.AddConstraintPolyline(StolUchoCurve2->p_IntersectionRes->GetFirstParameterPoints());

    //divDziura.DebugImages = true;
    divDziura.CreateDivision();

    auto swypeParams = divDziura.JoinConstraintPolylinesZigzag({1}, {}, true, true,
                                            1, 22);
    auto swypePrecPath = FromParams(swypeParams, StolOffsetK8.p_Intersection, K8_RADIUS);

    const float dziuraDu = 0.1f;
    const float dziuraDv = 0.03f;
    const float vStart = 7.9f;
    const float vEnd = 7.0f;
    const float uStart = StolUchoCurve2->p_IntersectionRes->GetFirstParameterPoints().front().x();
    const float uEnd = StolUchoCurve2->p_IntersectionRes->GetFirstParameterPoints().back().x();

    AddLinesAsConstrains(QVector2D(uStart, vStart), dziuraDv, dziuraDu,
                         vEnd,
                         QVector2D(uStart, uEnd), Y, divDziura);

    //divDziura.DebugImages = true;
    divDziura.CreateDivision();

    std::vector<int> dziuraLimit(12);
    std::iota(dziuraLimit.begin(), dziuraLimit.end(), 2);
    auto dziuraParameterPath = divDziura.JoinConstraintPolylinesZigzag(dziuraLimit, {0, 1}, true, true,
                                                                       2, 18);
    auto dziuraPrecPath = FromParams(dziuraParameterPath, StolOffsetK8.p_Intersection, K8_RADIUS);
#pragma endregion



    //Polaczenie wszystkich sciezek razem
    std::vector<QVector3D> resPrec;
    ConnectSecurelyTwoPathsPrec(resPrec, dziubekPrecPath, 0.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, bodyPrecPath1, 1.1f);
    ConnectSecurelyTwoPathsPrec(resPrec, uchoPrecPath, 1.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, swypePrecPath, 1.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, dziuraPrecPath, 0.2f);
    ConnectSecurelyTwoPathsPrec(resPrec, bodyPrecPath3, 1.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, bodyPrecPath2, 2.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, pokrywkaPrecPath3, 2.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, pokrywkaPrecPath1, 2.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, pokrywkaPrecPath2, 2.0f);
    ConnectSecurelyTwoPathsPrec(resPrec, swype2PrecPath, 2.0f);

    CutterPath precisionPath(CutterParameters(Length::FromSceneUnits(K8_RADIUS * 2), CutterType::Spherical));
    precisionPath.Points = AddSafeStartEndPosition(resPrec, {0.0f, 2.5f, 0.0f});

    const float tol = 0.01f;
    for (QVector3D &p: precisionPath.Points)
    {
        //Podniesienie o pol milimetra jesli schodzimy blisko podstawki
        if (p.y() < tol)
            p.setY(tol);
        p = FromSceneToBlock(p, blockWorldPos);
    }
    GCodeSaver::SaveCutterPath(folderName, precisionPath, 4);


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

std::vector<QVector3D>
RoutingAwareSystem::GenerateFlatPrecisionPath(const QVector3D &blockWorldPos, const QVector3D &blockSize)
{
    std::shared_ptr<RoutingAware> Body, Ucho, Pokrywka, Dziubek;
    auto elements = GetComponents();
    Body = elements[0].lock();
    Ucho = elements[1].lock();
    Dziubek = elements[2].lock();
    Pokrywka = elements[3].lock();

    PlaneCreator planeCreator("routingPlaneCreator", PLANEC2_CLASS, {-7.5f, 0.0f, -7.5f});
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

    OffsetPlane BodyOffsetF10(Body->p_Intersection, F10_RADIUS);
    OffsetPlane UchoOffsetF10(Ucho->p_Intersection, F10_RADIUS);
    OffsetPlane DziubekOffsetF10(Dziubek->p_Intersection, F10_RADIUS);
    OffsetPlane PokrywkaOffsetF10(Pokrywka->p_Intersection, F10_RADIUS);

    std::shared_ptr<IntersectionAwareSystem> isys;
    if (auto scene = SceneECS::Instance().lock())
        if (auto is = scene->GetSystem<IntersectionAwareSystem>().lock())
            isys = is;

    //Przeciecia stol-body
    QVector3D StolBodyStart1 = {-4.7, -0.3, 5.4}; //prawa strona
    auto StolBodyCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, BodyOffsetF10.p_Intersection,
                                                               StolBodyStart1);
    auto precFlat4 = StolBodyCurve1->p_IntersectionRes->GetScenePoints();
    QVector3D StolBodyStart2 = {-4.7, -0.3, -5.4}; //lewa strona
    auto StolBodyCurve2 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, BodyOffsetF10.p_Intersection,
                                                               StolBodyStart2);
    auto precFlat5 = StolBodyCurve2->p_IntersectionRes->GetScenePoints();

    //Przecięcia stol-ucho
    QVector3D StolUchoStart1 = {-3.2, -0.4, 4.3};
    auto StolUchoCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, UchoOffsetF10.p_Intersection,
                                                               StolUchoStart1);
    auto precFlat6 = StolUchoCurve1->p_IntersectionRes->GetScenePoints();

    //Przeciecia stol-dziubek
    QVector3D StolDziubekStart1 = {0.8, -0.4, -3.7};//gora
    auto StolDziubekCurve1 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, DziubekOffsetF10.p_Intersection,
                                                                  StolDziubekStart1);
    auto precFlat8 = StolDziubekCurve1->p_IntersectionRes->GetScenePoints();
    QVector3D StolDziubekStart2 = {-3.2, -0.4, -3.7};//dol
    auto StolDziubekCurve2 = isys->CreateIntersectionCurveBetween(stol->p_Intersection, DziubekOffsetF10.p_Intersection,
                                                                  StolDziubekStart2);
    auto precFlat7 = StolDziubekCurve2->p_IntersectionRes->GetScenePoints();

    //Przeciecia stol-pokrywka
    QVector3D StolPokrywkaStart1 = {3.1, -0.3, -2.1};//gora lewa
    QVector3D StolPokrywkaStart2 = {3.1, 0.3, 1.8};//gora prawa
    QVector3D StolPokrywkaStart3 = {2.1, -0.2, 1.8};//dol prawa
    QVector3D StolPokrywkaStart4 = {1.8, 1.0, -1.5};//dol lewa
    auto StolPokrywkaCurve1 = isys->CreateIntersectionCurveBetween(stol2->p_Intersection,
                                                                   PokrywkaOffsetF10.p_Intersection,
                                                                   StolPokrywkaStart1);
    auto StolPokrywkaCurve2 = isys->CreateIntersectionCurveBetween(stol2->p_Intersection,
                                                                   PokrywkaOffsetF10.p_Intersection,
                                                                   StolPokrywkaStart2);
    auto StolPokrywkaCurve3 = isys->CreateIntersectionCurveBetween(stol->p_Intersection,
                                                                   PokrywkaOffsetF10.p_Intersection,
                                                                   StolPokrywkaStart3);
    auto StolPokrywkaCurve4 = isys->CreateIntersectionCurveBetween(stol->p_Intersection,
                                                                   PokrywkaOffsetF10.p_Intersection,
                                                                   StolPokrywkaStart4);
    auto precFlat1 = StolPokrywkaCurve1->p_IntersectionRes->GetScenePoints();
    auto precFlat2 = StolPokrywkaCurve2->p_IntersectionRes->GetScenePoints();
    auto precFlat3 = StolPokrywkaCurve3->p_IntersectionRes->GetScenePoints();
    auto precFlat9 = StolPokrywkaCurve4->p_IntersectionRes->GetScenePoints();

    /*auto precFlatParams1 = StolPokrywkaCurve1->p_IntersectionRes->GetSecondParameterPoints();
    auto precFlatParams2 = StolPokrywkaCurve2->p_IntersectionRes->GetSecondParameterPoints();
    auto precFlatParams3 = StolPokrywkaCurve3->p_IntersectionRes->GetSecondParameterPoints();
    auto precFlatParams4 = StolPokrywkaCurve4->p_IntersectionRes->GetSecondParameterPoints();*/

    /*float constURight = 5.0f;
    float constVConstraintUpStart = 4.0f;
    float constVConstraintUpEnd = PokrywkaOffsetF10.p_UV->V - 1.f - 1e-2;
    float constULeft = 2.0f;
    float constVConstraintDownStart = 1.0f;
    float constVConstraintDownEnd = 2.0f + 1e-2;
    float dv = 0.1f;

    std::vector<QVector3D> precFlat1;//gora lewa
    std::vector<QVector3D> precFlat2;//gora prawa
    std::vector<QVector3D> precFlat3;//dol prawa
    std::vector<QVector3D> precFlat9;//dol lewa

    //Gora lewa
    for (float v = constVConstraintUpEnd; v > constVConstraintUpStart; v -= dv)

    {
        //Policzenie normalki w szczegolnym przypadku
        QVector3D derV = PokrywkaOffsetF10.p_Intersection->SceneFunctionDerV({constULeft, v}).normalized();
        QVector3D derU = {0.0f, -1.0f, 0.0f};
        QVector3D norm = QVector3D::crossProduct(derU, derV).normalized();
        precFlat2.emplace_back(Pokrywka->p_Intersection->SceneFunction({constULeft, v}) + (F10_RADIUS) * norm);
    }

    //Gora prawa
    for (float v = constVConstraintUpStart; v < constVConstraintUpEnd; v += dv)
    {
        //Policzenie normalki w szczegolnym przypadku
        QVector3D derV = PokrywkaOffsetF10.p_Intersection->SceneFunctionDerV({constURight, v}).normalized();
        QVector3D derU = {0.0f, 1.0f, 0.0f};
        QVector3D norm = QVector3D::crossProduct(derU, derV).normalized();
        precFlat1.emplace_back(Pokrywka->p_Intersection->SceneFunction({constURight, v}) + (F10_RADIUS) * norm);
    }

    //Dol lewa
    for (float v = constVConstraintDownEnd; v > constVConstraintDownStart; v -= dv)
    {
        //Policzenie normalki w szczegolnym przypadku
        QVector3D derV = PokrywkaOffsetF10.p_Intersection->SceneFunctionDerV({constULeft, v}).normalized();
        QVector3D derU = {0.0f, -1.0f, 0.0f};
        QVector3D norm = QVector3D::crossProduct(derU, derV).normalized();
        precFlat3.emplace_back(Pokrywka->p_Intersection->SceneFunction({constULeft, v}) + (F10_RADIUS) * norm);
    }

    //Dol prawa
    for (float v = constVConstraintDownStart; v < constVConstraintDownEnd; v += dv)
    {
        //Policzenie normalki w szczegolnym przypadku
        QVector3D derV = PokrywkaOffsetF10.p_Intersection->SceneFunctionDerV({constURight, v}).normalized();
        QVector3D derU = {0.0f, 1.0f, 0.0f};
        QVector3D norm = QVector3D::crossProduct(derU, derV).normalized();
        precFlat9.emplace_back(Pokrywka->p_Intersection->SceneFunction({constURight, v}) + (F10_RADIUS) * norm);
    }*/

    //Dodanie hardcodowanych punktów które wyszły tragicznie z przecięć
    auto f7back = precFlat7.back();
    auto f8front = precFlat8.front();
    precFlat7.emplace_back(QVector3D(f7back.x(), 0.0f, -7.25));
    precFlat7.emplace_back(QVector3D(f8front.x(), 0.0f, -7.25));

    precFlat1.erase(precFlat1.begin());

    precFlat2.pop_back();
    precFlat9.pop_back();

    precFlat2.emplace_back(QVector3D(2.9f, 0.0f, 3.2f));
    precFlat2.emplace_back(QVector3D(2.5f, 0.0f, 3.2f));
    precFlat9.emplace_back(QVector3D(2.5f, 0.0f, -2.6f));
    precFlat9.emplace_back(QVector3D(2.9f, 0.0f, -2.6f));

    QVector4D planeSize = QVector4D(
            blockWorldPos.x() - blockSize.x() / 2,
            blockWorldPos.z() - blockSize.y() / 2,
            blockWorldPos.x() + blockSize.x() / 2,
            blockWorldPos.z() + blockSize.y() / 2
    );
    PlaneDivision planeDiv(planeSize);
    planeDiv.AddConstraintPolyline(precFlat1);//gora lewa pokrywka
    planeDiv.AddConstraintPolyline(precFlat2);//gora prawa pokrywka
    planeDiv.AddConstraintPolyline(precFlat3);//dol prawa pokrywka
    planeDiv.AddConstraintPolyline(precFlat4);//body prawa
    planeDiv.AddConstraintPolyline(precFlat5);//body lewa
    planeDiv.AddConstraintPolyline(precFlat6);//ucho
    planeDiv.AddConstraintPolyline(precFlat7);//dol dziubek
    planeDiv.AddConstraintPolyline(precFlat8);//gora dziubek
    planeDiv.AddConstraintPolyline(precFlat9);//pokrywka dol lewa

    //planeDiv.DebugImages = true;
    planeDiv.CreateDivision();
    //planeDiv.DebugImages = false;
    auto offsetRing = planeDiv.JoinConstraintPolylinesTogetherInCycle();
    std::vector<QVector3D> offsetPoints;
    offsetPoints.reserve(offsetRing.size());

    for (const auto &planePoint: offsetRing)
        offsetPoints.emplace_back(QVector3D(planePoint.x(), 0, planePoint.y()));
    offsetPoints.emplace_back(offsetPoints[0]);

    //Wyczyszczenie zasobów
    stol->RemovePointsInside();
    stol2->RemovePointsInside();

    return offsetPoints;
}

int
RoutingAwareSystem::AddLinesAsConstrains(QVector2D startPoint, float deltaWidth, float deltaHeight, float targetWidth,
                                         QVector2D lineBoundaries, RoutingAwareSystem::ZigZagVariable variable,
                                         PlaneDivision &division, bool asCycles)
{
    int addedLines = 0;

    if (variable == X)
    {
        float incrementWidth = deltaWidth * (targetWidth - startPoint.x() > 0 ? 1 : -1);
        for (float x = startPoint.x(); incrementWidth > 0 ? x < targetWidth : x > targetWidth; x += incrementWidth)
        {
            std::vector<QVector2D> constraint;
            for (float y = lineBoundaries.x(); y <= lineBoundaries.y(); y += deltaHeight)
            {
                constraint.emplace_back(QVector2D(x, y));
            }

            division.AddConstraintPolyline(constraint, asCycles);
            addedLines++;
        }
    } else
    {
        float incrementWidth = deltaWidth * (targetWidth - startPoint.y() > 0 ? 1 : -1);
        for (float y = startPoint.y(); incrementWidth > 0 ? y < targetWidth : y > targetWidth; y += incrementWidth)
        {
            std::vector<QVector2D> constraint;
            for (float x = lineBoundaries.x(); x <= lineBoundaries.y(); x += deltaHeight)
                constraint.emplace_back(QVector2D(x, y));

            division.AddConstraintPolyline(constraint, asCycles);
            addedLines++;
        }
    }

    return addedLines;
}

std::vector<QVector3D>
RoutingAwareSystem::FromParams(const std::vector<QVector2D> &params, std::shared_ptr<IntersectionAware> plane,
                               float cutterRadius)
{
    std::vector<QVector3D> res(params.size());
    for (int i = 0; i < params.size(); ++i)
        res[i] = plane->SceneFunction(params[i]) - QVector3D(0.0f, cutterRadius, 0.0f);//Obnizenie do czubka frezu
    return res;
}

void
RoutingAwareSystem::ConnectSecurelyTwoPathsPrec(std::vector<QVector3D> &target, const std::vector<QVector3D> &addition,
                                                float sceneHeight)
{
    if (!target.empty())
    {
        target.emplace_back(QVector3D(target.back().x(), sceneHeight, target.back().z()));
        target.emplace_back(QVector3D(addition.front().x(), sceneHeight, addition.front().z()));
    }
    target.insert(target.end(), addition.begin(), addition.end());
}

std::vector<QVector3D> RoutingAwareSystem::AddSafeStartEndPosition(std::vector<QVector3D> &paths, QVector3D scenePosition)
{
    std::vector<QVector3D> res(paths.size() + 4);
    QVector3D startMid(paths.front().x(), scenePosition.y(), paths.front().z());
    QVector3D endMid(paths.back().x(), scenePosition.y(), paths.back().z());
    res[0] = scenePosition;
    res[1] = startMid;
    std::copy(paths.begin(), paths.end(), res.begin() + 2);
    res[res.size() - 2] = endMid;
    res[res.size() - 1] = scenePosition;
    return res;
}




