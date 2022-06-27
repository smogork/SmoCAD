#include "parametersintersectiondialog.h"
#include "ui_parametersintersectiondialog.h"

ParametersIntersectionDialog::ParametersIntersectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersIntersectionDialog)
{
    ui->setupUi(this);
}

ParametersIntersectionDialog::~ParametersIntersectionDialog()
{
    delete ui;
}

void ParametersIntersectionDialog::SetParamTextures(QImage &one, QImage &two)
{
    ui->lblParamsOne->setPixmap(QPixmap::fromImage(one));
    ui->lblParamsTwo->setPixmap(QPixmap::fromImage(two));
}
