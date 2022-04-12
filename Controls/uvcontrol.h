#ifndef UVCONTROL_H
#define UVCONTROL_H

#include <QWidget>

namespace Ui {
class UVControl;
}

class UVControl : public QWidget
{
    Q_OBJECT

public:
    explicit UVControl(QWidget *parent = nullptr);
    ~UVControl();

private:
    Ui::UVControl *ui;
};

#endif // UVCONTROL_H
