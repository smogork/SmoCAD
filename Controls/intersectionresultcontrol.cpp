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
        auto [oneTex, twoTex] = res->GetTrimmingTextures();
        paramsDialog->SetParamTextures(oneTex, twoTex);
        paramsDialog->show();
    }
}


void IntersectionResultControl::on_btnInterpolation_clicked()
{

}

