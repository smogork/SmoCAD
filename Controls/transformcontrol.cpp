#include "transformcontrol.h"
#include "ui_transformcontrol.h"

TransformControl::TransformControl(std::shared_ptr<Transform> t, QWidget *parent) :
        ComponentControl(parent), ui(new Ui::TransformControl)
{
    ui->setupUi(this);
    m_transform = t;

    this->ui->spinPosX->setValue(m_transform->Position.value().x());
    this->ui->spinPosY->setValue(m_transform->Position.value().y());
    this->ui->spinPosZ->setValue(m_transform->Position.value().z());
    this->ui->spinRotX->setValue(m_transform->Rotation.value().x());
    this->ui->spinRotY->setValue(m_transform->Rotation.value().y());
    this->ui->spinRotZ->setValue(m_transform->Rotation.value().z());
    this->ui->spinScaleX->setValue(m_transform->Scale.value().x());
    this->ui->spinScaleY->setValue(m_transform->Scale.value().y());
    this->ui->spinScaleZ->setValue(m_transform->Scale.value().z());
    ignoreValueChanged = false;

    m_positionNotifier = m_transform->Position.addNotifier([this]()
            {
                if (this->ignoreNotifier) return;
                ignoreValueChanged = true;
                this->ui->spinPosX->setValue(m_transform->Position.value().x());
                this->ui->spinPosY->setValue(m_transform->Position.value().y());
                this->ui->spinPosZ->setValue(m_transform->Position.value().z());
                ignoreValueChanged = false;
                emit RequestRepaint();
            });
    m_rotationNotifier = m_transform->Rotation.addNotifier([this]()
           {
               if (this->ignoreNotifier) return;
               ignoreValueChanged = true;
               this->ui->spinRotX->setValue(m_transform->Rotation.value().x());
               this->ui->spinRotY->setValue(m_transform->Rotation.value().y());
               this->ui->spinRotZ->setValue(m_transform->Rotation.value().z());
               ignoreValueChanged = false;
               emit RequestRepaint();

           });
    m_scaleNotifier = m_transform->Scale.addNotifier([this]()
           {
               if (this->ignoreNotifier) return;

               ignoreValueChanged = true;
               this->ui->spinScaleX->setValue(m_transform->Scale.value().x());
               this->ui->spinScaleY->setValue(m_transform->Scale.value().y());
               this->ui->spinScaleZ->setValue(m_transform->Scale.value().z());
               ignoreValueChanged = false;
               emit RequestRepaint();
           });
}

TransformControl::~TransformControl()
{
    delete ui;
}

void TransformControl::on_spinPosX_valueChanged(double arg1)
{
    UpdatePosition();
}

void TransformControl::on_spinPosY_valueChanged(double arg1)
{
    UpdatePosition();
}

void TransformControl::on_spinPosZ_valueChanged(double arg1)
{
    UpdatePosition();
}

void TransformControl::on_spinRotX_valueChanged(double arg1)
{
    UpdateRotation();
}

void TransformControl::on_spinRotY_valueChanged(double arg1)
{
    UpdateRotation();
}

void TransformControl::on_spinRotZ_valueChanged(double arg1)
{
    UpdateRotation();
}

void TransformControl::on_spinScaleX_valueChanged(double arg1)
{
    UpdateScale();
}

void TransformControl::on_spinScaleY_valueChanged(double arg1)
{
    UpdateScale();
}

void TransformControl::on_spinScaleZ_valueChanged(double arg1)
{
    UpdateScale();
}

void TransformControl::UpdatePosition()
{
    if (ignoreValueChanged)
        return;

    ignoreNotifier = true;
    m_transform->SetPosition(QVector3D(ui->spinPosX->value(), ui->spinPosY->value(), ui->spinPosZ->value()));
    ignoreNotifier = false;
    emit RequestRepaint();
}

void TransformControl::UpdateRotation()
{
    if (ignoreValueChanged)
        return;

    ignoreNotifier = true;
    m_transform->Rotation = QVector3D(ui->spinRotX->value(), ui->spinRotY->value(), ui->spinRotZ->value());
    ignoreNotifier = false;
    emit RequestRepaint();
}

void TransformControl::UpdateScale()
{
    if (ignoreValueChanged)
        return;

    ignoreNotifier = true;
    m_transform->Scale = QVector3D(ui->spinScaleX->value(), ui->spinScaleY->value(), ui->spinScaleZ->value());
    ignoreNotifier = false;
    emit RequestRepaint();
}
