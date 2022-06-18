#include "uvcontrol.h"
#include "ui_uvcontrol.h"

UVControl::UVControl(std::shared_ptr<UVParams> uv, QWidget *parent) :
        ComponentControl(parent),
    ui(new Ui::UVControl)
{
    ui->setupUi(this);
    m_uv = uv;

    this->ui->spinParamU->setValue(m_uv->U);
    this->ui->spinParamV->setValue(m_uv->V);
    this->ui->spinParamUDens->setValue(m_uv->UDensity);
    this->ui->spinParamVDens->setValue(m_uv->VDensity);
    ignoreValueChanged = false;

    if (m_uv->IsUVLocked())
    {
        this->ui->spinParamU->setEnabled(false);
        this->ui->spinParamV->setEnabled(false);
    }

    m_uNotifier = m_uv->U.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamU->setValue(m_uv->U); });
    m_vNotifier = m_uv->V.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamV->setValue(m_uv->V); });
    m_udNotifier = m_uv->UDensity.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamUDens->setValue(m_uv->UDensity); });
    m_vdNotifier = m_uv->VDensity.addNotifier([this]() {if (this->ignoreNotifier) return; this->ui->spinParamVDens->setValue(m_uv->VDensity); });
}

UVControl::~UVControl()
{
    delete ui;
}

void UVControl::on_spinParamU_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->U = ui->spinParamU->value());
}


void UVControl::on_spinParamUDens_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->UDensity = ui->spinParamUDens->value());
}


void UVControl::on_spinParamV_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->V = ui->spinParamV->value());
}


void UVControl::on_spinParamVDens_valueChanged(int arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(m_uv->VDensity = ui->spinParamVDens->value());
}
