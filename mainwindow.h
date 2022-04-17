#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QSpacerItem>

#include <Renderer/InputController/InputController.h>
#include "Objects/IRenderableObject.h"
#include "Objects/CursorObject.h"
#include "Scene/SceneModelOld.h"
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
    std::list<std::unique_ptr<ComponentControl>> componentControls;
    std::unique_ptr<QSpacerItem> componentSpacer;

    void CreateCursorOnScene(std::shared_ptr<SceneMouseClickEvent> event);
    void UpdateCursorUI(QVector3D wPos, QPoint vPos);
    void UpdateCursorWorldPosition();
    void UpdateCursorViewPosition();
    QPoint GetCursorViewPosition();
    void BlockCursorUISignals(bool b);
    void ResizeEvent(QSize size);

protected slots:
    void MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event);
    void CameraUpdated(std::shared_ptr<CameraUpdateEvent> event);
    void UpdateComponentUI(unsigned int oid);

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

    void on_spinCurPosX_valueChanged(double arg1);
    void on_spinCurPosY_valueChanged(double arg1);
    void on_spinCurPosZ_valueChanged(double arg1);
    void on_spinCurViewPosX_valueChanged(int arg1);
    void on_spinCurViewPosY_valueChanged(int arg1);

    void on_actionTorus_triggered();
    void on_actionPoint_triggered();
    void on_actionCube_triggered();
    void on_actionBezierC0_triggered();
    void on_actionBezierC2_triggered();

    void on_actionShow_Bezier_polygon_toggled(bool arg1);
};
#endif // MAINWINDOW_H
