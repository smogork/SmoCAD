#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Entities/Point.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Scene/Entities/BezierC2.h"
#include "Scene/Entities/Cube.h"
#include "Scene/Entities/BezierC0.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&Renderer::controller, &InputController::SceneMouseClicked,
                     this, &MainWindow::MouseRaycastSlot);
    QObject::connect(&Renderer::controller, &InputController::CameraUpdated,
                     this, &MainWindow::CameraUpdated);
    QObject::connect(ui->sceneWidget, &GLWidget::WidgetResized,
                     this, &MainWindow::ResizeEvent);

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

void MainWindow::CreateCursorOnScene(std::shared_ptr<SceneMouseClickEvent> event)
{
    QVector4D plain = Renderer::controller.Camera->GetCenterViewPlain();

    float t = -QVector4D::dotProduct(plain, event->RaycastStart) /
              QVector4D::dotProduct(plain, event->RaycastDirection);

    QVector3D clickPoint = (event->RaycastDirection * t + event->RaycastStart).toVector3D();

    /*qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;*/

    //model->UpdateCursor(clickPoint);
    UpdateCursorUI(clickPoint, event->ViewClickPoint);
}

#pragma region CursorUiEvents

void MainWindow::on_spinCurPosX_valueChanged(double arg1)
{
    UpdateCursorWorldPosition();
}


void MainWindow::on_spinCurPosY_valueChanged(double arg1)
{
    UpdateCursorWorldPosition();
}


void MainWindow::on_spinCurPosZ_valueChanged(double arg1)
{
    UpdateCursorWorldPosition();
}


void MainWindow::on_spinCurViewPosX_valueChanged(int arg1)
{
    UpdateCursorViewPosition();
}


void MainWindow::on_spinCurViewPosY_valueChanged(int arg1)
{
    UpdateCursorViewPosition();
}

void MainWindow::UpdateCursorUI(QVector3D wPos, QPoint vPos)
{
    BlockCursorUISignals(true);
    /*ui->spinCurPosX->setValue(wPos.x());
    ui->spinCurPosY->setValue(wPos.y());
    ui->spinCurPosZ->setValue(wPos.z());
    ui->spinCurViewPosX->setValue(vPos.x());
    ui->spinCurViewPosY->setValue(vPos.y());*/
    BlockCursorUISignals(false);
}

void MainWindow::BlockCursorUISignals(bool b)
{
    /*ui->spinCurPosX->blockSignals(b);
    ui->spinCurPosY->blockSignals(b);
    ui->spinCurPosZ->blockSignals(b);
    ui->spinCurViewPosX->blockSignals(b);
    ui->spinCurViewPosY->blockSignals(b);*/
}

void MainWindow::UpdateCursorWorldPosition()
{
    //model->UpdateCursor(QVector3D(ui->spinCurPosX->value(), ui->spinCurPosY->value(), ui->spinCurPosZ->value()));
    CameraUpdated(nullptr);
}

void MainWindow::UpdateCursorViewPosition()
{
    //Renderer::controller.EmitSceneMouseClickedEvent(QPoint(ui->spinCurViewPosX->value(), ui->spinCurViewPosY->value()), false);
    ui->sceneWidget->update();
}

QPoint MainWindow::GetCursorViewPosition()
{
    /*QVector3D pos = model->GetCursorObject()->Position;
    QVector3D vPoint = pos.project(
            controls->Camera->GetViewMatrix(),
            viewport->GetProjectionMatrix(),
            QRect(QPoint(0.0f, 0.0f), viewport->GetViewportSize()));
    return QPoint(vPoint.x(), viewport->GetViewportSize().height() - vPoint.y());*/
    return {};
}

void MainWindow::CameraUpdated(std::shared_ptr<CameraUpdateEvent> event)
{
    /*if (model->GetCursorObject())
    {
        QPoint vPos = GetCursorViewPosition();
        BlockCursorUISignals(true);
        ui->spinCurViewPosX->setValue(vPos.x());
        ui->spinCurViewPosY->setValue(vPos.y());
        BlockCursorUISignals(false);
        ui->sceneWidget->update();
    }*/
}

void MainWindow::ResizeEvent(QSize size)
{
    /*ui->spinCurViewPosX->setMaximum(size.width());
    ui->spinCurViewPosY->setMaximum(size.height());*/
}

#pragma endregion

void MainWindow::AddPointToBezier()
{
    /*BezierCurveC0* bezier = dynamic_cast<BezierCurveC0*>(model->GetSelectedObject());

    for (QListWidgetItem* i : ui->listWidgetObjects->selectedItems())
    {
        auto item = dynamic_cast<QListWidgetRenderableItem*>(i);

        if (item)
        {
            bezier->AddControlPoint(dynamic_cast<PointObject*>(item->obj));
            ui->sceneWidget->update();
        }
    }*/
}

void MainWindow::CreateBezierFromPoints()
{
    /*BezierCurveC0* bezier =  new BezierCurveC0();
    connect(this, &MainWindow::PointObjectChanged, bezier, &BezierCurveC0::onPointChanged);
    AddNewObject(bezier, "BezierC0", true);*/

    /*for (QListWidgetItem* i : ui->listWidgetObjects->selectedItems())
    {
        auto item = dynamic_cast<QListWidgetRenderableItem*>(i);

        if (item)
            bezier->AddControlPoint(dynamic_cast<PointObject*>(item->obj));
    }*/

    ui->sceneWidget->update();
}

void MainWindow::on_actionShow_Bezier_polygon_toggled(bool arg1)
{
    //model->ShowBezeierPolygon = arg1;
    ui->sceneWidget->update();
}



