#ifndef TRANSFORMCONTROL_H
#define TRANSFORMCONTROL_H

#include <QWidget>
#include "Scene/Components/Transform.h"

namespace Ui {
class TransformControl;
}

class TransformControl : public QWidget
{
    Q_OBJECT

public:
    explicit TransformControl(std::shared_ptr<Transform> t, QWidget *parent = nullptr);
    ~TransformControl();

signals:
    void RequestRepaint();

private slots:
    void on_spinPosX_valueChanged(double arg1);
    void on_spinPosY_valueChanged(double arg1);
    void on_spinPosZ_valueChanged(double arg1);
    void on_spinRotX_valueChanged(double arg1);
    void on_spinRotY_valueChanged(double arg1);
    void on_spinRotZ_valueChanged(double arg1);
    void on_spinScaleX_valueChanged(double arg1);
    void on_spinScaleY_valueChanged(double arg1);
    void on_spinScaleZ_valueChanged(double arg1);

private:
    Ui::TransformControl *ui;
    std::shared_ptr<Transform> m_transform;
    QPropertyNotifier m_positionNotifier;
    QPropertyNotifier m_rotationNotifier;
    QPropertyNotifier m_scaleNotifier;
    bool ignoreNotifier = false;
    bool ignoreValueChanged = true;

    void UpdatePosition();
    void UpdateRotation();
    void UpdateScale();
};

#endif // TRANSFORMCONTROL_H
