#include <QListWidget>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"
#include "Objects/TorusObject.h"
#include "Controls/QListWidgetRenderableItem.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = std::make_unique<SceneModel>();
    viewport = std::make_shared<Viewport>(ui->sceneWidget->size(), 60);
    controls = std::make_shared<InputController>(viewport, this);
    ui->sceneWidget->SetupSceneAndControls(controls, model);

    QObject::connect(this->controls.get(), &InputController::SceneMouseClicked,
                     this, &MainWindow::MouseRaycastSlot);

    for (auto ro : model->GetRenderableObjects())
        listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "dupa_start", ro));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTorus_triggered()
{
    auto ro = new TorusObject(QVector3D(), 5, 1, 36, 18);
    model->AddObject(ro);
    listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "torus_dupa", ro));
    ui->sceneWidget->update();
}

void MainWindow::on_actionPoint_triggered()
{
    auto ro = new PointObject(QVector3D());
    model->AddObject(ro);
    listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "punkt_dupa", ro));
    ui->sceneWidget->update();
}

void MainWindow::on_actionDelete_triggered()
{

}

void MainWindow::MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event)
{
    QVector4D plain = controls->Camera->GetCenterViewPlain();

    float t = -QVector4D::dotProduct(plain, event->RaycastStart) /
              QVector4D::dotProduct(plain, event->RaycastDirection);

    QVector3D clickPoint = (event->RaycastDirection * t + event->RaycastStart).toVector3D();

    /*qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;*/

    model->UpdateCursor(clickPoint);
    ui->sceneWidget->update();
}

