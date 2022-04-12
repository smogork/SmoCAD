#include "transformcollectioncontrol.h"
#include "ui_transformcollectioncontrol.h"

TransformCollectionControl::TransformCollectionControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformCollectionControl)
{
    ui->setupUi(this);
}

TransformCollectionControl::~TransformCollectionControl()
{
    delete ui;
}
