#include "cursorcontrol.h"
#include "ui_cursorcontrol.h"

CursorControl::CursorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CursorControl)
{
    ui->setupUi(this);
}

CursorControl::~CursorControl()
{
    delete ui;
}
