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
    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &MainWindow::CameraUpdated);
    QObject::connect(this->model.get(), &SceneModel::SelectedObjectChanged,
                     this, &MainWindow::SelectObjectChanged);

    for (auto ro : model->GetRenderableObjects())
        listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "dupa_start", ro, model));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTorus_triggered()
{
    auto ro = new TorusObject(QVector3D(), 5, 1, 36, 18);
    model->AddObject(ro);
    listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "Torus", ro, model));
    ui->sceneWidget->update();
}

void MainWindow::on_actionPoint_triggered()
{
    auto ro = new PointObject(QVector3D());
    model->AddObject(ro);
    listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "Point", ro, model));
    ui->sceneWidget->update();
}

void MainWindow::on_actionDelete_triggered()
{
    auto selectedObjects = model->GetSelectedObjects();
    for (auto o : selectedObjects)
    {
        listObjects.remove_if(
                [&](std::unique_ptr<QListWidgetRenderableItem> &item)
                {
                    return item->CompareInsideObject(o);
                }
        );
        model->RemoveObject(o);
    }
    ui->sceneWidget->update();
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
    UpdateCursorUI(clickPoint, event->ViewClickPoint);

    ui->sceneWidget->update();
}

void MainWindow::on_listWidgetObjects_itemClicked(QListWidgetItem *item)
{
    auto rItem = (QListWidgetRenderableItem*)item;
    rItem->SelectOnScene(ui->listWidgetObjects->selectedItems().size() > 1);
    ui->sceneWidget->update();
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
    ui->spinCurPosX->setValue(wPos.x());
    ui->spinCurPosY->setValue(wPos.y());
    ui->spinCurPosZ->setValue(wPos.z());
    ui->spinCurViewPosX->setValue(vPos.x());
    ui->spinCurViewPosY->setValue(vPos.y());
    BlockCursorUISignals(false);
}

void MainWindow::BlockCursorUISignals(bool b)
{
    ui->spinCurPosX->blockSignals(b);
    ui->spinCurPosY->blockSignals(b);
    ui->spinCurPosZ->blockSignals(b);
    ui->spinCurViewPosX->blockSignals(b);
    ui->spinCurViewPosY->blockSignals(b);
}

void MainWindow::UpdateCursorWorldPosition()
{
    model->UpdateCursor(QVector3D(ui->spinCurPosX->value(), ui->spinCurPosY->value(), ui->spinCurPosZ->value()));
    CameraUpdated(nullptr);
}

void MainWindow::UpdateCursorViewPosition()
{
    controls->EmitSceneMouseClickedEvent(QPoint(ui->spinCurViewPosX->value(), ui->spinCurViewPosY->value()));
    ui->sceneWidget->update();
}

QPoint MainWindow::GetCursorViewPosition()
{
    QVector3D pos = model->GetCursorObject()->Position;
    QVector3D vPoint = pos.project(
            controls->Camera->GetViewMatrix(),
            viewport->GetProjectionMatrix(),
            QRect(QPoint(0.0f, 0.0f), viewport->GetViewportSize()));
    return QPoint(vPoint.x(), viewport->GetViewportSize().height() - vPoint.y());
}

void MainWindow::CameraUpdated(std::shared_ptr<CameraUpdateEvent> event)
{
    if (model->GetCursorObject())
    {
        QPoint vPos = GetCursorViewPosition();
        BlockCursorUISignals(true);
        ui->spinCurViewPosX->setValue(vPos.x());
        ui->spinCurViewPosY->setValue(vPos.y());
        BlockCursorUISignals(false);
        ui->sceneWidget->update();
    }
}
#pragma endregion

#pragma region TransformUIEvents
void MainWindow::on_spinPosX_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinPosY_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinPosZ_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinRotX_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinRotY_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinRotZ_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinScaleX_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinScaleY_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::on_spinScaleZ_valueChanged(double arg1)
{
    UpdateSelectedObject();
}

void MainWindow::UpdateSelectedObject()
{
    auto selectedObjects = model->GetSelectedObjects();
    if (selectedObjects.size() == 1)
    {
        IRenderableObject* item = selectedObjects.front();
        item->Position = QVector3D(ui->spinPosX->value(), ui->spinPosY->value(), ui->spinPosZ->value());
        item->Rotation = QVector3D(ui->spinRotX->value(), ui->spinRotY->value(), ui->spinRotZ->value());
        item->Scale = QVector3D(ui->spinScaleX->value(), ui->spinScaleY->value(), ui->spinScaleZ->value());
    }

    ui->sceneWidget->update();
}

void MainWindow::SelectObjectChanged(std::shared_ptr<SelectedObjectChangedEvent> event)
{
    auto selectedObjects = model->GetSelectedObjects();
    if (selectedObjects.size() == 1)
    {
        IRenderableObject* item = selectedObjects.front();
        BlockTransformUISignals(true);
        ui->spinPosX->setValue(item->Position.x());
        ui->spinPosY->setValue(item->Position.y());
        ui->spinPosZ->setValue(item->Position.z());
        ui->spinRotX->setValue(item->Rotation.x());
        ui->spinRotY->setValue(item->Rotation.y());
        ui->spinRotZ->setValue(item->Rotation.z());
        ui->spinScaleX->setValue(item->Scale.x());
        ui->spinScaleY->setValue(item->Scale.y());
        ui->spinScaleZ->setValue(item->Scale.z());
        BlockTransformUISignals(false);
    }

    ui->sceneWidget->update();
}

void MainWindow::BlockTransformUISignals(bool b)
{
    ui->spinPosX->blockSignals(b);
    ui->spinPosY->blockSignals(b);
    ui->spinPosZ->blockSignals(b);
    ui->spinRotX->blockSignals(b);
    ui->spinRotY->blockSignals(b);
    ui->spinRotZ->blockSignals(b);
    ui->spinScaleX->blockSignals(b);
    ui->spinScaleY->blockSignals(b);
    ui->spinScaleZ->blockSignals(b);
}
#pragma endregion

void MainWindow::on_spinParamU_valueChanged(double arg1)
{

}


void MainWindow::on_spinParamUDens_valueChanged(int arg1)
{

}


void MainWindow::on_spinParamV_valueChanged(double arg1)
{

}


void MainWindow::on_spinParamVDens_valueChanged(int arg1)
{

}















