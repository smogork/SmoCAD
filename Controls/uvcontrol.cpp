#include "uvcontrol.h"
#include "ui_uvcontrol.h"

UVControl::UVControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UVControl)
{
    ui->setupUi(this);
}

UVControl::~UVControl()
{
    delete ui;
}
