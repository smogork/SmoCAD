#ifndef TRANSFORMCOLLECTIONCONTROL_H
#define TRANSFORMCOLLECTIONCONTROL_H

#include <QWidget>

namespace Ui {
class TransformCollectionControl;
}

class TransformCollectionControl : public QWidget
{
    Q_OBJECT

public:
    explicit TransformCollectionControl(QWidget *parent = nullptr);
    ~TransformCollectionControl();

private:
    Ui::TransformCollectionControl *ui;
};

#endif // TRANSFORMCOLLECTIONCONTROL_H
