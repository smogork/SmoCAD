#include <QListWidget>
#include <QInputDialog>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Entities/Point.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Systems/SceneElementSystem.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Scene/Entities/BezierC2.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateComponentUI(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        componentControls = scene->CreateUIForObject(oid);
        for (const std::unique_ptr<ComponentControl> &widget: componentControls)
        {
            ui->verticalLayout->addWidget(widget.get());
            QObject::connect(widget.get(), &ComponentControl::RequestRepaint,
                             ui->sceneWidget, &GLWidget::RedrawScreen);
        }
    }
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
    //AddNewObject(new TorusObject(QVector3D(), 5, 1, 36, 18), "Torus");
}

void MainWindow::on_actionPoint_triggered()
{
    std::shared_ptr<Point> p = std::make_shared<Point>("NewPoint", QVector3D(0,0,0));
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObject(p);

        if (auto select = scene->GetSystem<SelectableSystem>().lock())
        {
            if (auto obj = select->GetSelectedObject())
            {
                auto oid = obj->GetAttachedObjectID();
                auto col = scene->GetSystem<TransformCollectionSystem>().lock();
                if (auto colection = col->GetComponent(oid).lock())
                {
                    colection->AddPoint(p->p_CollectionAware);
                }
            }
        }
    }
    ui->sceneWidget->update();



    /*Torus* test = new Torus(QVector3D(1, 2, 3));


    test->p_Transform->Rotation = QVector3D(90, 0, -90);
    test->p_Transform->Scale = QVector3D(1, 1, 2);
    UpdateComponentUI(test->GetObjectID());*/


    //PointObject* pointObject = new PointObject(QVector3D());
    //AddNewObject(pointObject, "Point");
    /*BezierCurveC0* bezier = dynamic_cast<BezierCurveC0*>(model->GetSelectedObject());
    if (bezier)
        bezier->AddControlPoint(pointObject);*/
}

void MainWindow::on_actionCube_triggered()
{
    //AddNewObject(new CubeObject(QVector3D()), "Cube");
}

void MainWindow::on_actionBezierC0_triggered()
{
    /*BezierCurveC0* bezier =  new BezierCurveC0();
    connect(this, &MainWindow::PointObjectChanged, bezier, &BezierCurveC0::onPointChanged);
    AddNewObject(bezier, "VirtualBezierC0", true);*/

    std::shared_ptr<BezierC2> bezier = std::make_shared<BezierC2>("NEwBezierC2");
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObject(bezier);
    }
    ui->sceneWidget->update();
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

    selectedTransform = nullptr;*/
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



