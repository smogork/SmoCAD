#include <QListWidget>
#include <QInputDialog>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"
#include "Scene/Systems/SelectableSystem.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidgetObjects->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidgetObjects, &QListWidget::customContextMenuRequested, this, &MainWindow::showObjectListContextMenu);

    //model = std::make_unique<SceneModelOld>();
    viewport = std::make_shared<Viewport>(ui->sceneWidget->size(), 60);
    controls = std::make_shared<InputController>(viewport, this);

    ui->sceneWidget->SetupSceneAndControls(controls);

    QObject::connect(this->controls.get(), &InputController::SceneMouseClicked,
                     this, &MainWindow::MouseRaycastSlot);
    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &MainWindow::CameraUpdated);
    //QObject::connect(this->model.get(), &SceneModelOld::SelectedObjectChanged,
                     //this, &MainWindow::SelectObjectChanged);
    QObject::connect(ui->sceneWidget, &GLWidget::WidgetResized,
                     this, &MainWindow::ResizeEvent);



    //for (auto ro : model->GetRenderableObjects())
    //    listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, "Start objects", ro, model));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddNewObject(IRenderableObject* ro, const QString& name, bool positionless)
{
    /*if (model->GetCursorObject() || positionless)
    {
        if (model->AddObject(ro, positionless))
        {
            listObjects.push_back(std::make_unique<QListWidgetRenderableItem>(ui->listWidgetObjects, name, ro, model));
            ui->sceneWidget->update();
        }
    }
    else
        delete ro;*/
}

void MainWindow::on_actionTorus_triggered()
{
    AddNewObject(new TorusObject(QVector3D(), 5, 1, 36, 18), "Torus");
}

void MainWindow::on_actionPoint_triggered()
{
    PointObject* pointObject = new PointObject(QVector3D());
    AddNewObject(pointObject, "Point");
    /*BezierCurveC0* bezier = dynamic_cast<BezierCurveC0*>(model->GetSelectedObject());
    if (bezier)
        bezier->AddControlPoint(pointObject);*/
}

void MainWindow::on_actionCube_triggered()
{
    AddNewObject(new CubeObject(QVector3D()), "Cube");
}

void MainWindow::on_actionBezierC0_triggered()
{
    BezierCurveC0* bezier =  new BezierCurveC0();
    connect(this, &MainWindow::PointObjectChanged, bezier, &BezierCurveC0::onPointChanged);
    AddNewObject(bezier, "BezierC0", true);
}

void MainWindow::on_actionRename_triggered()
{
    /*IRenderableObject* selected = model->GetSelectedObject();
    if (selected)
    {
        bool ok;
        //[TODO] stowrzyc strukture aby kazdy obiekt mial nazwe
        QString newName = QInputDialog::getText(this, "Rename object", "Insert new name of object", QLineEdit::Normal, "", &ok);

        if (!ok)
            return;

        auto found = std::find_if(listObjects.begin(), listObjects.end(),
                [&](std::unique_ptr<QListWidgetRenderableItem> &item)
                {
                    return item->CompareInsideObject(selected);
                }
        );
        if (found != listObjects.end())
        {
            (*found)->setText(newName);
        }
    }*/
}

void MainWindow::on_actionDelete_triggered()
{
    /*if (model->GetSelectedObject())
    {
        listObjects.remove_if(
                [&](std::unique_ptr<QListWidgetRenderableItem> &item)
                {
                    return item->CompareInsideObject(model->GetSelectedObject());
                }
        );

        PointObject* point = dynamic_cast<PointObject*>(model->GetSelectedObject());
        if (point)
        {
            std::shared_ptr<PointObjectChangedEvent> event = std::make_shared<PointObjectChangedEvent>(point, true);
            emit PointObjectChanged(event);
        }
        model->RemoveObject(model->GetSelectedObject());
    }
    else if (model->GetCompositeObject())
    {
        for (CompositeObject::CompositeTransform& o: model->GetCompositeObject()->GetObjects())
        {
            listObjects.remove_if(
                    [&](std::unique_ptr<QListWidgetRenderableItem> &item)
                    {
                        return item->CompareInsideObject(o.Object);
                    }
            );
        }
        model->RemoveComposite();
    }

    selectedTransform = nullptr;
    ui->sceneWidget->update();*/
}

void MainWindow::MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->MouseClicked(event);
        ui->sceneWidget->update();
    }

    /*if (model->SelectObjectByMouse(event->RaycastStart, event->RaycastDirection))
    {
        ui->groupBoxTransform->setEnabled(true);
        ui->groupBoxUVParams->setEnabled(dynamic_cast<TorusObject*>(selectedTransform) != nullptr);
        ui->groupBoxCursor->setEnabled(false);

        model->DeleteCursor();
    }
    else
    {
        if (event->UnselectObjects)
        {
            ui->groupBoxTransform->setEnabled(false);
            ui->groupBoxUVParams->setEnabled(false);
            ui->groupBoxCursor->setEnabled(true);

            model->UnselectObjects();
        }
        CreateCursorOnScene(event);
    }

    ui->sceneWidget->update();*/
}

void MainWindow::CreateCursorOnScene(std::shared_ptr<SceneMouseClickEvent> event)
{
    QVector4D plain = controls->Camera->GetCenterViewPlain();

    float t = -QVector4D::dotProduct(plain, event->RaycastStart) /
              QVector4D::dotProduct(plain, event->RaycastDirection);

    QVector3D clickPoint = (event->RaycastDirection * t + event->RaycastStart).toVector3D();

    /*qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;*/

    //model->UpdateCursor(clickPoint);
    UpdateCursorUI(clickPoint, event->ViewClickPoint);
}

void MainWindow::on_listWidgetObjects_itemClicked(QListWidgetItem *item)
{
    auto rItem = (QListWidgetRenderableItem*)item;

    if (rItem->SelectOnScene(ui->listWidgetObjects->selectedItems().size() > 1))
    {
        ui->groupBoxTransform->setEnabled(true);
        ui->groupBoxUVParams->setEnabled(dynamic_cast<TorusObject*>(rItem->obj) != nullptr);

        ui->sceneWidget->update();
    }
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
    //model->UpdateCursor(QVector3D(ui->spinCurPosX->value(), ui->spinCurPosY->value(), ui->spinCurPosZ->value()));
    CameraUpdated(nullptr);
}

void MainWindow::UpdateCursorViewPosition()
{
    controls->EmitSceneMouseClickedEvent(QPoint(ui->spinCurViewPosX->value(), ui->spinCurViewPosY->value()), false);
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
    if (selectedTransform)
    {
        selectedTransform->Position = QVector3D(ui->spinPosX->value(), ui->spinPosY->value(), ui->spinPosZ->value());
        selectedTransform->Rotation = QVector3D(ui->spinRotX->value(), ui->spinRotY->value(), ui->spinRotZ->value());
        selectedTransform->Scale = QVector3D(ui->spinScaleX->value(), ui->spinScaleY->value(), ui->spinScaleZ->value());

        UpdateUVParamsOfObject(dynamic_cast<TorusObject*>(selectedTransform));

        PointObject* point = dynamic_cast<PointObject*>(selectedTransform);
        if (point)
        {
            std::shared_ptr<PointObjectChangedEvent> event = std::make_shared<PointObjectChangedEvent>(point, false);
            emit PointObjectChanged(event);
        }
    }

    ui->sceneWidget->update();
}

void MainWindow::SelectObjectChanged(std::shared_ptr<SelectedObjectChangedEvent> event)
{
    selectedTransform = event->ObjectToTransform;
    BlockTransformUISignals(true);
    ui->spinPosX->setValue(selectedTransform->Position.x());
    ui->spinPosY->setValue(selectedTransform->Position.y());
    ui->spinPosZ->setValue(selectedTransform->Position.z());
    ui->spinRotX->setValue(selectedTransform->Rotation.x());
    ui->spinRotY->setValue(selectedTransform->Rotation.y());
    ui->spinRotZ->setValue(selectedTransform->Rotation.z());
    ui->spinScaleX->setValue(selectedTransform->Scale.x());
    ui->spinScaleY->setValue(selectedTransform->Scale.y());
    ui->spinScaleZ->setValue(selectedTransform->Scale.z());
    BlockTransformUISignals(false);

    UpdateUVParamsOfControls(dynamic_cast<TorusObject*>(selectedTransform));

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


/*void MainWindow::EnableTransformContorls(bool b)
{
    ui->spinPosX->setEnabled(b);
    ui->spinPosY->setEnabled(b);
    ui->spinPosZ->setEnabled(b);
    ui->spinRotX->setEnabled(b);
    ui->spinRotY->setEnabled(b);
    ui->spinRotZ->setEnabled(b);
    ui->spinScaleX->setEnabled(b);
    ui->spinScaleY->setEnabled(b);
    ui->spinScaleZ->setEnabled(b);
}*/
#pragma endregion

#pragma region UVParamsUIEvents
void MainWindow::on_spinParamU_valueChanged(double arg1)
{
    UpdateSelectedObject();
}


void MainWindow::on_spinParamUDens_valueChanged(int arg1)
{
    UpdateSelectedObject();
}


void MainWindow::on_spinParamV_valueChanged(double arg1)
{
    UpdateSelectedObject();
}


void MainWindow::on_spinParamVDens_valueChanged(int arg1)
{
    UpdateSelectedObject();
}

void MainWindow::UpdateUVParamsOfObject(TorusObject* UVObject)
{
    if (UVObject)
    {
        UVObject->SetBiggerRadius(ui->spinParamU->value());
        UVObject->SetSmallerRadius(ui->spinParamV->value());
        UVObject->SetBiggerRadiusDensity(ui->spinParamUDens->value());
        UVObject->SetSmallerRadiusDensity(ui->spinParamVDens->value());
    }
}

void MainWindow::BlockUVParamUISignals(bool b)
{
    ui->spinParamU->blockSignals(b);
    ui->spinParamV->blockSignals(b);
    ui->spinParamUDens->blockSignals(b);
    ui->spinParamVDens->blockSignals(b);
}

void MainWindow::UpdateUVParamsOfControls(TorusObject *UVObject)
{
    if (UVObject)
    {
        BlockUVParamUISignals(true);

        ui->spinParamU->setValue(UVObject->GetBiggerR());
        ui->spinParamV->setValue(UVObject->GetSmallerR());
        ui->spinParamUDens->setValue(UVObject->GetBiggerRDensity());
        ui->spinParamVDens->setValue(UVObject->GetSmallerRDensity());

        BlockUVParamUISignals(false);
    }
}

void MainWindow::ResizeEvent(QSize size)
{
    ui->spinCurViewPosX->setMaximum(size.width());
    ui->spinCurViewPosY->setMaximum(size.height());
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

void MainWindow::showObjectListContextMenu(const QPoint &pos)
{
// Handle global position
    /*QPoint globalPos = ui->listWidgetObjects->mapToGlobal(pos);

    int selectedPoints = 0;
    for (QListWidgetItem* i : ui->listWidgetObjects->selectedItems())
        selectedPoints += dynamic_cast<QListWidgetRenderableItem*>(i) ? 1 : 0;

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Remove", this, &MainWindow::on_actionDelete_triggered);
    if (dynamic_cast<BezierCurveC0*>(model->GetSelectedObject()))
        myMenu.addAction("Add to bezier", this, &MainWindow::AddPointToBezier);
    if (selectedPoints > 1)
        myMenu.addAction("Create bezier from points", this, &MainWindow::CreateBezierFromPoints);
    //myMenu.addAction("Erase",  this, SLOT(eraseItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);*/
}

void MainWindow::CreateBezierFromPoints()
{
    BezierCurveC0* bezier =  new BezierCurveC0();
    connect(this, &MainWindow::PointObjectChanged, bezier, &BezierCurveC0::onPointChanged);
    AddNewObject(bezier, "BezierC0", true);

    for (QListWidgetItem* i : ui->listWidgetObjects->selectedItems())
    {
        auto item = dynamic_cast<QListWidgetRenderableItem*>(i);

        if (item)
            bezier->AddControlPoint(dynamic_cast<PointObject*>(item->obj));
    }

    ui->sceneWidget->update();
}

void MainWindow::on_actionShow_Bezier_polygon_toggled(bool arg1)
{
    //model->ShowBezeierPolygon = arg1;
    ui->sceneWidget->update();
}

