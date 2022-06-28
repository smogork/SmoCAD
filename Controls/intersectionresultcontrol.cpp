#include "intersectionresultcontrol.h"
#include "ui_intersectionresultcontrol.h"

IntersectionResultControl::IntersectionResultControl(std::shared_ptr<IntersectionResult> res, QWidget *parent) :
        ComponentControl(parent),
    ui(new Ui::IntersectionResultControl)
{
    ui->setupUi(this);
    m_res = res;
}

IntersectionResultControl::~IntersectionResultControl()
{
    delete ui;
}


void IntersectionResultControl::on_btnDomains_clicked()
{
    if (auto res = m_res.lock())
    {
        paramsDialog = std::make_unique<ParametersIntersectionDialog>();
        auto one = res->GetTrimmingTextureOne();
        auto two = res->GetTrimmingTextureTwo();
        paramsDialog->SetParamTextures(one, two);
        paramsDialog->show();
    }
}


void IntersectionResultControl::on_btnInterpolation_clicked()
{

}

