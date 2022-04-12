#include "transformcontorol.h"
#include "ui_transformcontorol.h"

TransformContorol::TransformContorol(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformContorol)
{
    ui->setupUi(this);
}

TransformContorol::~TransformContorol()
{
    delete ui;
}
