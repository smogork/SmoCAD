#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <Renderer/InputController/InputController.h>
#include "Objects/IRenderableObject.h"
#include "Objects/CursorObject.h"
#include "Scene/SceneModelOld.h"
#include "Controls/QListWidgetRenderableItem.h"
#include "Objects/TorusObject.h"
#include "Scene/SceneECS.h"
#include "Controls/transformcontrol.h"
#include "Controls/uvcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    //std::shared_ptr<SceneModelOld> model = nullptr;
    //std::shared_ptr<SceneECS> model = nullptr;
    std::list<std::unique_ptr<QListWidgetRenderableItem>> listObjects;
    TransformableObject* selectedTransform = nullptr;
    std::unique_ptr<TransformControl> transformTest;
    std::unique_ptr<UVControl> UVTest;

    void AddNewObject(IRenderableObject* ro, const QString& name, bool positionless = false);

    void CreateCursorOnScene(std::shared_ptr<SceneMouseClickEvent> event);
    void UpdateCursorUI(QVector3D wPos, QPoint vPos);
    void UpdateCursorWorldPosition();
    void UpdateCursorViewPosition();
    QPoint GetCursorViewPosition();
    void UpdateSelectedObject();

    void UpdateUVParamsOfObject(TorusObject* UVObject);
    void UpdateUVParamsOfControls(TorusObject* UVObject);

    void BlockCursorUISignals(bool b);
    void BlockTransformUISignals(bool b);
    void BlockUVParamUISignals(bool b);
    void ResizeEvent(QSize size);

protected slots:
    void MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event);
    void CameraUpdated(std::shared_ptr<CameraUpdateEvent> event);
    void SelectObjectChanged(std::shared_ptr<SelectedObjectChangedEvent> event);

    void showObjectListContextMenu(const QPoint& pos);
    void AddPointToBezier();
    void CreateBezierFromPoints();

protected:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE { Renderer::controller.keyPressSlot(event);}
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE { Renderer::controller.keyReleaseSlot(event);}

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void PointObjectChanged(std::shared_ptr<PointObjectChangedEvent> event);

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

    void on_actionShow_Bezier_polygon_toggled(bool arg1);

    void on_actionRename_triggered();
    void on_actionBezierC0_triggered();
};
#endif // MAINWINDOW_H
