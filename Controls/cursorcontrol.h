#ifndef CURSORCONTROL_H
#define CURSORCONTROL_H

#include <QWidget>

namespace Ui {
class CursorControl;
}

class CursorControl : public QWidget
{
    Q_OBJECT

public:
    explicit CursorControl(QWidget *parent = nullptr);
    ~CursorControl();

private:
    Ui::CursorControl *ui;
};

#endif // CURSORCONTROL_H
