#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"
#include "Objects/TorusObject.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTorus_triggered()
{
    if (model->Cursor)
        model->AddObject(new TorusObject(model->Cursor->Position, 5, 1, 36, 18));
}

void MainWindow::on_actionPoint_triggered()
{
    if (model->Cursor)
        model->AddObject(new PointObject(model->Cursor->Position));
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

    if (model->Cursor)
        model->Cursor->Position = clickPoint;
    else
    {
        model->Cursor = std::make_shared<CursorObject>(clickPoint);
    }

    ui->sceneWidget->update();
}

