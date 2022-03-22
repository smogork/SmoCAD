#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTorus_triggered()
{
    ui->sceneWidget->AddSelectableObject(new TorusObject(QVector3D(), 5, 1, 36, 18));
}

void MainWindow::on_actionPoint_triggered()
{
    ui->sceneWidget->AddSelectableObject(new PointObject(QVector3D()));
}

void MainWindow::on_actionDelete_triggered()
{

}

