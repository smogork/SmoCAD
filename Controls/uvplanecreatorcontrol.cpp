#include "uvplanecreatorcontrol.h"
#include "ui_uvplanecreatorcontrol.h"
#include "Scene/SceneECS.h"

UVPlaneCreatorControl::~UVPlaneCreatorControl()
{
    delete ui;
}

UVPlaneCreatorControl::UVPlaneCreatorControl(std::shared_ptr<UVPlaneCreator> uv, QWidget *parent) :
        ComponentControl(parent),
        ui(new Ui::UVPlaneCreatorControl)
{
    ui->setupUi(this);
    m_uv = uv;

    this->ui->spinParamU->setValue(m_uv->U);
    this->ui->spinParamV->setValue(m_uv->V);
    this->ui->spinParamUDens->setValue(m_uv->UDensity);
    this->ui->spinParamVDens->setValue(m_uv->VDensity);
    this->ui->spinWidth->setValue(m_uv->Width);
    this->ui->spinHeight->setValue(m_uv->Height);
    ignoreValueChanged = false;

    m_uNotifier = m_uv->U.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamU->setValue(m_uv->U); });
    m_vNotifier = m_uv->V.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamV->setValue(m_uv->V); });
    m_udNotifier = m_uv->UDensity.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamUDens->setValue(m_uv->UDensity); });
    m_vdNotifier = m_uv->VDensity.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamVDens->setValue(m_uv->VDensity); });
    m_wNotifier = m_uv->U.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinWidth->setValue(m_uv->Width); });
    m_hNotifier = m_uv->V.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinHeight->setValue(m_uv->Height); });
}

void UVPlaneCreatorControl::on_spinParamU_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->U = ui->spinParamU->value());
}

void UVPlaneCreatorControl::on_spinParamUDens_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->UDensity = ui->spinParamUDens->value());
}

void UVPlaneCreatorControl::on_spinParamV_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->V = ui->spinParamV->value());
}

void UVPlaneCreatorControl::on_spinParamVDens_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->VDensity = ui->spinParamVDens->value());
}

void UVPlaneCreatorControl::on_checkBox_toggled(bool checked)
{
    //[TODO] dodac zawiniecie
}

void UVPlaneCreatorControl::on_btnCreatePlain_clicked()
{
    //[TODO] zmienic aby tworzyla sie rozna plaszczyzna pozniej
    auto plain = m_uv->CreatePlainC0("Cokolwiek");
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObject(plain);
        scene->RemoveObject(m_uv->GetAttachedObjectID());
        emit RequestRepaint();
        emit RequestControlsUpdate(SceneECS::NON_OBJECT_ID);
    }
}


void UVPlaneCreatorControl::on_spinWidth_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->Width = ui->spinWidth->value());
}


void UVPlaneCreatorControl::on_spinHeight_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->Height = ui->spinHeight->value());
}

