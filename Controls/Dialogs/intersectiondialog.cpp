#include "intersectiondialog.h"
#include "ui_intersectiondialog.h"

IntersectionDialog::IntersectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntersectionDialog)
{
    ui->setupUi(this);
}

IntersectionDialog::~IntersectionDialog()
{
    delete ui;
}

float IntersectionDialog::PointsSceneDistance()
{
    return ui->doubleSpinBox->value();
}

bool IntersectionDialog::StartFromCursor()
{
    return ui->chkCursorStart->isChecked();
}

int IntersectionDialog::SearchingStartDensity()
{
    return ui->spinDensity->value();
}
