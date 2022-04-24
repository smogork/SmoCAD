#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Cube.h"
#include "Scene/Entities/Curves/BezierC0.h"

#include "Renderer/Options.h"
#include "Scene/Entities/Curves/InterpolationC2.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&Renderer::controller, &InputController::SceneMouseClicked,
                     this, &MainWindow::MouseRaycastSlot);

    //register signals to cursorControl
    QObject::connect(ui->sceneWidget, &GLWidget::WidgetResized,
                     ui->cursorPosWidget, &CursorControl::ViewportResized);
    QObject::connect(ui->cursorPosWidget, &CursorControl::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);

    //register signals to SceneListElements
    QObject::connect(ui->sceneElementsWIdget, &SceneElementsList::RequestControlsUpdate,
                     this, &MainWindow::UpdateComponentUI);
    QObject::connect(ui->sceneElementsWIdget, &SceneElementsList::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);

    componentSpacer.reset();
    componentSpacer = std::make_unique<QSpacerItem>(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout->insertSpacerItem(0, componentSpacer.get());
}

MainWindow::~MainWindow()
{
    ui->verticalLayout->takeAt(0);
    componentSpacer.reset();
    componentControls.clear();
    delete ui;
}

void MainWindow::UpdateComponentUI(unsigned int oid)
{

    if (auto scene = SceneECS::Instance().lock())
    {
        componentControls = scene->CreateUIForObject(oid);
        for (const std::unique_ptr<ComponentControl> &widget: componentControls)
        {
            //for (const std::unique_ptr<ComponentControl> &widget: scene->CreateUIForObject(oid)) {
            widget->setParent(ui->scrollAreaWidgetContents);
            ui->verticalLayout->addWidget(widget.get());
            QObject::connect(widget.get(), &ComponentControl::RequestRepaint,
                             ui->sceneWidget, &GLWidget::RedrawScreen);
        }
    }
}

#pragma region Creating new objects
void MainWindow::on_actionTorus_triggered()
{
    std::shared_ptr<Torus> t = std::make_shared<Torus>("NewTorus");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(t);
    ui->sceneWidget->update();
}

void MainWindow::on_actionPoint_triggered()
{
    std::shared_ptr<Point> p = std::make_shared<Point>("NewPoint", QVector3D(0, 0, 0));
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObject(p);

        //Jezeli aktualnie wybrany obiekt jest kolekcja punktow - dodaj do niej
        if (auto select = scene->GetSystem<SelectableSystem>().lock())
        {
            if (auto obj = select->GetSelectedObject())
            {
                auto oid = obj->GetAttachedObjectID();
                if (auto colection = scene->GetComponentOfSystem<TransformCollectionSystem, TransformCollection>(oid)
                        .lock())
                {
                    colection->AddPoint(p->p_CollectionAware);
                }
            }
        }
    }
    ui->sceneWidget->update();
}

void MainWindow::on_actionCube_triggered()
{
    std::shared_ptr<Cube> c = std::make_shared<Cube>("NewCube");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(c);
    ui->sceneWidget->update();
}

void MainWindow::on_actionBezierC0_triggered()
{

    std::shared_ptr<BezierC0> b0 = std::make_shared<BezierC0>("NewBezierC0");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(b0);
    ui->sceneWidget->update();
}

void MainWindow::on_actionBezierC2_triggered()
{
    std::shared_ptr<BezierC2> b2 = std::make_shared<BezierC2>("NewBezierC2");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(b2);
    ui->sceneWidget->update();
}

void MainWindow::on_actionInterpolationC2_triggered()
{
    std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("NewInterpolationC2");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(i2);
    ui->sceneWidget->update();
}

void MainWindow::MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        unsigned int oid = scene->MouseClicked(event);
        UpdateComponentUI(oid);
        ui->sceneWidget->update();
    }
}
#pragma endregion

void MainWindow::on_actionShow_Bezier_polygon_toggled(bool arg1)
{
    Options::DrawBezierPolygon = arg1;
    ui->sceneWidget->update();
}

void MainWindow::on_actionShow_BSpline_polygon_toggled(bool arg1)
{
    Options::DrawDeBoorPolygon = arg1;
    ui->sceneWidget->update();
}

