#ifndef STEREOSCOPICCONFIG_H
#define STEREOSCOPICCONFIG_H

#include <QWidget>
#include "ComponentControl.h"

namespace Ui {
class StereoscopicConfig;
}

class StereoscopicConfig :  public ComponentControl
{
    Q_OBJECT

public:
    explicit StereoscopicConfig(QWidget *parent = nullptr);
    ~StereoscopicConfig();

private slots:
    void on_spinEyeSep_valueChanged(double arg1);
    void on_spinFocusDist_valueChanged(double arg1);

private:
    Ui::StereoscopicConfig *ui;
};

#endif // STEREOSCOPICCONFIG_H
