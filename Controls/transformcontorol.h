#ifndef TRANSFORMCONTOROL_H
#define TRANSFORMCONTOROL_H

#include <QWidget>

namespace Ui {
class TransformContorol;
}

class TransformContorol : public QWidget
{
    Q_OBJECT

public:
    explicit TransformContorol(QWidget *parent = nullptr);
    ~TransformContorol();

private:
    Ui::TransformContorol *ui;
};

#endif // TRANSFORMCONTOROL_H
