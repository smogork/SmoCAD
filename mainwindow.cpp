#include "mainwindow.h"
#include "./ui_mainwindow.h"

/*Trzeba zrobić:
 * 2. Zastapic szescian torusem.
 * 3. Polaczyc kontrolki z parametrami torusa.
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

