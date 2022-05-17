#ifndef UVPLANECREATORCONTROL_H
#define UVPLANECREATORCONTROL_H

#include <QWidget>
#include "ComponentControl.h"
#include "Scene/Components/UvPlainCreator.h"

namespace Ui {
class UVPlaneCreatorControl;
}

class UVPlaneCreatorControl : public ComponentControl
{
Q_OBJECT

public:
    explicit UVPlaneCreatorControl(std::shared_ptr<UVPlaneCreator> uv, QWidget *parent = nullptr);
    ~UVPlaneCreatorControl() override;

private slots:
    void on_spinParamU_valueChanged(double arg1);
    void on_spinParamUDens_valueChanged(int arg1);
    void on_spinParamV_valueChanged(double arg1);
    void on_spinParamVDens_valueChanged(int arg1);

    void on_checkBox_toggled(bool checked);
    void on_btnCreatePlain_clicked();

private:
    Ui::UVPlaneCreatorControl *ui;
    std::shared_ptr<UVPlaneCreator> m_uv;
    QPropertyNotifier m_uNotifier;
    QPropertyNotifier m_vNotifier;
    QPropertyNotifier m_udNotifier;
    QPropertyNotifier m_vdNotifier;


};

#endif // UVPLANECREATORCONTROL_H
