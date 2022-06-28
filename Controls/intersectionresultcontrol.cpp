#include "intersectionresultcontrol.h"
#include "ui_intersectionresultcontrol.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/SceneECS.h"

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
    if (auto res = m_res.lock())
    {
        if (auto scene = SceneECS::Instance().lock())
        {
            std::vector<std::shared_ptr<CollectionAware>> ps;
            ps.reserve(res->GetScenePointsSize());

            int i = 0;
            for (const QVector3D& coord : res->GetScenePoints())
            {
                auto p = std::make_shared<Point>(QString("InterpolationIntersectionCurve_P%0").arg(i++), coord);
                scene->AddObjectExplicitPosition(p);
                ps.push_back(p->p_CollectionAware);
            }
            std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("InterpolationIntersectionCurve");


            scene->AddObject(i2);
            i2->p_Collection->SetPoints(ps);
            i2->p_Selected->Selected = true;

            emit RequestRepaint();
            emit RequestControlsUpdate(i2->GetObjectID());
        }
    }
}

