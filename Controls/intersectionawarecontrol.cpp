#include "intersectionawarecontrol.h"
#include "ui_intersectionawarecontrol.h"

IntersectionAwareControl::IntersectionAwareControl(std::shared_ptr<IntersectionAware> aware, QWidget *parent) :
        ComponentControl(parent),
    ui(new Ui::IntersectionAwareControl)
{
    ui->setupUi(this);
    m_aware = aware;
}

IntersectionAwareControl::~IntersectionAwareControl()
{
    delete ui;
}

void IntersectionAwareControl::on_btnFlip_clicked()
{
    if (auto aware = m_aware.lock())
    {
        aware->FlipTrimming = !aware->FlipTrimming;
        emit RequestRepaint();
    }
}

