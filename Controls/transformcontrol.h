#ifndef TRANSFORMCONTROL_H
#define TRANSFORMCONTROL_H

#include <QWidget>
#include "Scene/Components/Transform.h"
#include "ComponentControl.h"

namespace Ui {
class TransformControl;
}

class TransformControl : public ComponentControl
{
    Q_OBJECT

public:
    explicit TransformControl(std::shared_ptr<Transform> t, QWidget *parent = nullptr);
    ~TransformControl();

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

    void UpdatePosition();
    void UpdateRotation();
    void UpdateScale();
};

#endif // TRANSFORMCONTROL_H
