#include "stereoscopicconfig.h"
#include "ui_stereoscopicconfig.h"

#include "Renderer/Options.h"

StereoscopicConfig::StereoscopicConfig(QWidget *parent) :
        ComponentControl(parent),
    ui(new Ui::StereoscopicConfig)
{
    ui->setupUi(this);

    ui->spinEyeSep->setValue(Options::StereoParams->EyeSeparation);
    ui->spinFocusDist->setValue(Options::StereoParams->FocusDistance);
    ignoreValueChanged = false;
}

StereoscopicConfig::~StereoscopicConfig()
{
    delete ui;
}

void StereoscopicConfig::on_spinEyeSep_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(Options::StereoParams->EyeSeparation = arg1);
}


void StereoscopicConfig::on_spinFocusDist_valueChanged(double arg1)
{
    UPDATE_VALUE_IGNORING_NOTIFIER(Options::StereoParams->FocusDistance = arg1);
}

