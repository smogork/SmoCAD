#ifndef UVCONTROL_H
#define UVCONTROL_H

#include <QWidget>
#include "Scene/Components/UVParams.h"
#include "ComponentControl.h"

namespace Ui {
class UVControl;
}

class UVControl : public ComponentControl
{
    Q_OBJECT

public:
    explicit UVControl(std::shared_ptr<UVParams> uv, QWidget *parent = nullptr);
    ~UVControl();

private slots:
    void on_spinParamU_valueChanged(double arg1);
    void on_spinParamUDens_valueChanged(int arg1);
    void on_spinParamV_valueChanged(double arg1);
    void on_spinParamVDens_valueChanged(int arg1);

private:
    Ui::UVControl *ui;
    std::shared_ptr<UVParams> m_uv;
    QPropertyNotifier m_uNotifier;
    QPropertyNotifier m_vNotifier;
    QPropertyNotifier m_udNotifier;
    QPropertyNotifier m_vdNotifier;

};

#endif // UVCONTROL_H
