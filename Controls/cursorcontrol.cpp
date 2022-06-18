#include "cursorcontrol.h"
#include "ui_cursorcontrol.h"
#include "Scene/SceneECS.h"

#include <QVector3D>

CursorControl::CursorControl(QWidget *parent) :
        ComponentControl(parent),
        ui(new Ui::CursorControl)
{
    ui->setupUi(this);
    ignoreValueChanged = false;

    QObject::connect(&Renderer::controller, &InputController::CameraUpdated,
                     this, &CursorControl::CameraUpdated);
    if (auto scene = SceneECS::Instance().lock())
    {
        QObject::connect(scene.get(), &SceneECS::CursorChange,
                         this, &CursorControl::UpdateCursor);
    }
}

CursorControl::~CursorControl()
{
    delete ui;
}

void CursorControl::UpdateCursor(std::shared_ptr<Cursor> cur)
{
    m_weak_cursor = cur;

    std::shared_ptr<Cursor> m_cursor = cur;

    ui->groupBoxCursor->setEnabled(cur != nullptr);
    if (cur)
    {
        cursorWorldPosNotifier = cur->p_Transform->Position.addNotifier([this]()
        {
            if (this->ignoreNotifier) return;

            std::shared_ptr<Cursor> m_cursor = m_weak_cursor.lock();
            if (!m_cursor)
                return;

            UPDATE_UI_IGNORING_VALUE_CHANGED(
                    ui->spinCurPosX->setValue(m_cursor->p_Transform->Position.value().x());
                    ui->spinCurPosY->setValue(m_cursor->p_Transform->Position.value().y());
                    ui->spinCurPosZ->setValue(m_cursor->p_Transform->Position.value().z())
            );

            SetScreenPosFromWorldPos();
        });

        UPDATE_UI_IGNORING_VALUE_CHANGED(
                ui->spinCurPosX->setValue(m_cursor->p_Transform->Position.value().x());
                ui->spinCurPosY->setValue(m_cursor->p_Transform->Position.value().y());
                ui->spinCurPosZ->setValue(m_cursor->p_Transform->Position.value().z())
        );
        SetScreenPosFromWorldPos();
    }
}

void CursorControl::SetScreenPosFromWorldPos()
{
    std::shared_ptr<Cursor> m_cursor = m_weak_cursor.lock();
    if (!m_cursor)
        return;

    QPoint vPos = GetCursorViewPosition(m_cursor->p_Transform->Position);
    UPDATE_UI_IGNORING_VALUE_CHANGED(
            ui->spinCurViewPosX->setValue(vPos.x());
            ui->spinCurViewPosY->setValue(vPos.y());
    );
}

void CursorControl::CameraUpdated(std::shared_ptr<CameraUpdateEvent> event)
{
    std::shared_ptr<Cursor> m_cursor = m_weak_cursor.lock();
    if (!m_cursor)
        return;

    SetScreenPosFromWorldPos();
}

void CursorControl::ViewportResized(QSize size)
{
    std::shared_ptr<Cursor> m_cursor = m_weak_cursor.lock();
    if (!m_cursor)
        return;

    ui->spinCurViewPosX->setMaximum(size.width());
    ui->spinCurViewPosY->setMaximum(size.height());
    if (m_cursor)
        SetScreenPosFromWorldPos();
}

void CursorControl::on_spinCurPosX_valueChanged(double arg1)
{
    UpdateFromWorldPosition();
}

void CursorControl::on_spinCurPosY_valueChanged(double arg1)
{
    UpdateFromWorldPosition();
}

void CursorControl::on_spinCurPosZ_valueChanged(double arg1)
{
    UpdateFromWorldPosition();
}

void CursorControl::on_spinCurViewPosX_valueChanged(int arg1)
{
    UpdateFromScreenPosition();
}

void CursorControl::on_spinCurViewPosY_valueChanged(int arg1)
{
    UpdateFromScreenPosition();
}

void CursorControl::UpdateFromWorldPosition()
{
    std::shared_ptr<Cursor> m_cursor = m_weak_cursor.lock();
    if (!m_cursor)
        return;

    UPDATE_VALUE_IGNORING_NOTIFIER_NOREPAINT(
            m_cursor->p_Transform->Position = QVector3D(ui->spinCurPosX->value(), ui->spinCurPosY->value(),
                                                        ui->spinCurPosZ->value())
    );
    SetScreenPosFromWorldPos();
    emit RequestRepaint();
}

void CursorControl::UpdateFromScreenPosition()
{
    UPDATE_VALUE_IGNORING_NOTIFIER(
            Renderer::controller.EmitCursorFromScreenEvent(
                    QPoint(ui->spinCurViewPosX->value(), ui->spinCurViewPosY->value()))
    );
}

QPoint CursorControl::GetCursorViewPosition(QVector3D worldPos)
{
    QVector3D vPoint = worldPos.project(
            Renderer::controller.Camera->GetViewMatrix(),
            Renderer::controller.viewport->GetProjectionMatrix(),
            QRect(QPoint(0.0f, 0.0f), Renderer::controller.viewport->GetViewportSize()));
    return QPoint(vPoint.x(), Renderer::controller.viewport->GetViewportSize().height() - vPoint.y());
}



