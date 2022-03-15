#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "KeyboardMouseHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_spinTorusR_valueChanged(double arg1);

    void on_spinTorusr_valueChanged(double arg1);

    void on_spinTorusRDensity_valueChanged(int arg1);

    void on_spinTorusrDensity_valueChanged(int arg1);

    void on_spinPosX_valueChanged(double arg1);

    void on_spinPosY_valueChanged(double arg1);

    void on_spinPosZ_valueChanged(double arg1);

    void on_spinRotX_valueChanged(double arg1);

    void on_spinRotY_valueChanged(double arg1);

    void on_spinRotZ_valueChanged(double arg1);

    void on_spinScaleX_valueChanged(double arg1);

    void on_spinScaleY_valueChanged(double arg1);

    void on_spinScaleZ_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
