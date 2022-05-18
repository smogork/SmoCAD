#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QSpacerItem>

#include <Renderer/InputController/InputController.h>
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

protected:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE { Renderer::controller.keyPressSlot(event); }
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE { Renderer::controller.keyReleaseSlot(event);}

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event);
    void UpdateComponentUI(unsigned int oid);

    void on_actionTorus_triggered();
    void on_actionPoint_triggered();
    void on_actionCube_triggered();
    void on_actionBezierC0_triggered();
    void on_actionBezierC2_triggered();
    void on_actionInterpolationC2_triggered();

    void on_actionShow_Bezier_polygon_toggled(bool arg1);
    void on_actionShow_BSpline_polygon_toggled(bool arg1);
    void on_actionShow_Bezier_mesh_triggered(bool checked);
    void on_actionAnaglyphic_3D_view_toggled(bool arg1);
};
#endif // MAINWINDOW_H
