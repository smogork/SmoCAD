#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <Renderer/InputController/InputController.h>
#include "Objects/IRenderableObject.h"
#include "Objects/CursorObject.h"
#include "Scene/SceneModel.h"
#include "Controls/QListWidgetRenderableItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    std::shared_ptr<InputController> controls = nullptr;
    std::shared_ptr<Viewport> viewport = nullptr;
    std::shared_ptr<SceneModel> model = nullptr;
    std::list<std::unique_ptr<QListWidgetRenderableItem>> listObjects;
    TransformableObject* selectedTransform = nullptr;

    void AddNewObject(IRenderableObject* ro, const QString& name);

    void CreateCursorOnScene(std::shared_ptr<SceneMouseClickEvent> event);
    void UpdateCursorUI(QVector3D wPos, QPoint vPos);
    void UpdateCursorWorldPosition();
    void UpdateCursorViewPosition();
    void BlockCursorUISignals(bool b);
    QPoint GetCursorViewPosition();

    void UpdateSelectedObject();
    void BlockTransformUISignals(bool b);

protected slots:
    void MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event);
    void CameraUpdated(std::shared_ptr<CameraUpdateEvent> event);
    void SelectObjectChanged(std::shared_ptr<SelectedObjectChangedEvent> event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_spinPosX_valueChanged(double arg1);
    void on_spinPosY_valueChanged(double arg1);
    void on_spinPosZ_valueChanged(double arg1);
    void on_spinRotX_valueChanged(double arg1);
    void on_spinRotY_valueChanged(double arg1);
    void on_spinRotZ_valueChanged(double arg1);
    void on_spinScaleX_valueChanged(double arg1);
    void on_spinScaleY_valueChanged(double arg1);
    void on_spinScaleZ_valueChanged(double arg1);

    void on_spinCurPosX_valueChanged(double arg1);
    void on_spinCurPosY_valueChanged(double arg1);
    void on_spinCurPosZ_valueChanged(double arg1);
    void on_spinCurViewPosX_valueChanged(int arg1);
    void on_spinCurViewPosY_valueChanged(int arg1);

    void on_spinParamU_valueChanged(double arg1);
    void on_spinParamUDens_valueChanged(int arg1);
    void on_spinParamV_valueChanged(double arg1);
    void on_spinParamVDens_valueChanged(int arg1);

    void on_listWidgetObjects_itemClicked(QListWidgetItem *item);
    void on_actionTorus_triggered();
    void on_actionPoint_triggered();
    void on_actionDelete_triggered();
    void on_actionCube_triggered();
};
#endif // MAINWINDOW_H
