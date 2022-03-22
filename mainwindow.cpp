#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Objects/PointObject.h"

/*Trzeba zrobić:
 * 2. Zastapic szescian torusem.
 * 3. Polaczyc kontrolki z parametrami torusa.
 */

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}


/*void MainWindow::on_spinTorusR_valueChanged(double arg1)
{
    ui->sceneWidget->UpdateTorusObjectParameters(
            arg1,
            ui->sceneWidget->torus->GetSmallerR(),
            ui->sceneWidget->torus->GetBiggerRDensity(),
            ui->sceneWidget->torus->GetSmallerRDensity()
    );
}


void MainWindow::on_spinTorusr_valueChanged(double arg1)
{
    ui->sceneWidget->UpdateTorusObjectParameters(
            ui->sceneWidget->torus->GetBiggerR(),
            arg1,
            ui->sceneWidget->torus->GetBiggerRDensity(),
            ui->sceneWidget->torus->GetSmallerRDensity()
    );
}


void MainWindow::on_spinTorusRDensity_valueChanged(int arg1)
{
    ui->sceneWidget->UpdateTorusObjectParameters(
            ui->sceneWidget->torus->GetBiggerR(),
            ui->sceneWidget->torus->GetSmallerR(),
            arg1,
            ui->sceneWidget->torus->GetSmallerRDensity()
    );
}


void MainWindow::on_spinTorusrDensity_valueChanged(int arg1)
{
    ui->sceneWidget->UpdateTorusObjectParameters(
            ui->sceneWidget->torus->GetBiggerR(),
            ui->sceneWidget->torus->GetSmallerR(),
            ui->sceneWidget->torus->GetBiggerRDensity(),
            arg1
    );
}


void MainWindow::on_spinPosX_valueChanged(double arg1)
{
    QVector3D change(
            arg1,
            ui->sceneWidget->torus->Position.y(),
            ui->sceneWidget->torus->Position.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            change,
            ui->sceneWidget->torus->Rotation,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinPosY_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Position.x(),
            arg1,
            ui->sceneWidget->torus->Position.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            change,
            ui->sceneWidget->torus->Rotation,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinPosZ_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Position.x(),
            ui->sceneWidget->torus->Position.y(),
            arg1
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            change,
            ui->sceneWidget->torus->Rotation,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinRotX_valueChanged(double arg1)
{
    QVector3D change(
            arg1,
            ui->sceneWidget->torus->Rotation.y(),
            ui->sceneWidget->torus->Rotation.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            change,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinRotY_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Rotation.x(),
            arg1,
            ui->sceneWidget->torus->Rotation.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            change,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinRotZ_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Rotation.x(),
            ui->sceneWidget->torus->Rotation.y(),
            arg1
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            change,
            ui->sceneWidget->torus->Scale
    );
}


void MainWindow::on_spinScaleX_valueChanged(double arg1)
{
    QVector3D change(
            arg1,
            ui->sceneWidget->torus->Scale.y(),
            ui->sceneWidget->torus->Scale.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            ui->sceneWidget->torus->Rotation,
            change
    );
}


void MainWindow::on_spinScaleY_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Scale.x(),
            arg1,
            ui->sceneWidget->torus->Scale.z()
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            ui->sceneWidget->torus->Rotation,
            change
    );
}


void MainWindow::on_spinScaleZ_valueChanged(double arg1)
{
    QVector3D change(
            ui->sceneWidget->torus->Scale.x(),
            ui->sceneWidget->torus->Scale.y(),
            arg1
    );

    ui->sceneWidget->UpdateTorusObjectTransform(
            ui->sceneWidget->torus->Position,
            ui->sceneWidget->torus->Rotation,
            change
    );
}*/


void MainWindow::on_actionTorus_triggered()
{
    ui->sceneWidget->AddSelectableObject(new TorusObject(QVector3D(), ui->sceneWidget->shader, 5, 1, 36, 18));
}


void MainWindow::on_actionPoint_triggered()
{
    ui->sceneWidget->AddSelectableObject(new PointObject(QVector3D(), ui->sceneWidget->shader));
}


void MainWindow::on_actionDelete_triggered()
{

}

