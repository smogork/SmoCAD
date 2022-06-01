#ifndef CURSORCONTROL_H
#define CURSORCONTROL_H

#include <QWidget>
#include "Scene/Entities/Cursor.h"
#include "ComponentControl.h"

namespace Ui {
class CursorControl;
}

class CursorControl : public ComponentControl
{
    Q_OBJECT

public:
    explicit CursorControl(QWidget *parent = nullptr);
    ~CursorControl() override;

public slots:
    void ViewportResized(QSize size);

private slots:
    void UpdateCursor(std::shared_ptr<Cursor> cur);
    void CameraUpdated(std::shared_ptr<CameraUpdateEvent> event);

    void on_spinCurPosX_valueChanged(double arg1);
    void on_spinCurPosY_valueChanged(double arg1);
    void on_spinCurPosZ_valueChanged(double arg1);
    void on_spinCurViewPosX_valueChanged(int arg1);
    void on_spinCurViewPosY_valueChanged(int arg1);

private:
    Ui::CursorControl *ui;
    std::weak_ptr<Cursor> m_weak_cursor;
    QPropertyNotifier cursorWorldPosNotifier;

    void UpdateFromWorldPosition();
    void UpdateFromScreenPosition();
    void SetScreenPosFromWorldPos();
    QPoint GetCursorViewPosition(QVector3D worldPos);
};

#endif // CURSORCONTROL_H
