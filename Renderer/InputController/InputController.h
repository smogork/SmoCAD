#ifndef KeyboardMouseHandlerH
#define KeyboardMouseHandlerH

#include "Renderer/InputController/InputEvents/CameraUpdateEvent.h"
#include "Renderer/InputController/InputEvents/SceneMouseClickEvent.h"
#include "Renderer/Camera/Viewport.h"
#include "Renderer/InputController/InputEvents/ObjectMoveEvent.h"
#include "Renderer/InputController/InputEvents/SelectRectangleUpdateEvent.h"

#include <QKeyEvent>
#include <QObject>
#include <QPoint>
#include <QTimer>
#include <set>
#include <map>

#include <Renderer/Camera/OrbitalCamera.h>

#define UNDEFINED_ID -1
#define LMOUSE_ID 0
#define MMOUSE_ID 1
#define RMOUSE_ID 2

#define MOVE_SENSITIVITY 0.005f
#define KEYBOARD_MOVE_SENSITIVITY 0.05f
#define ROTATE_SENSITIVITY 0.005f
#define ZOOM_SENSITIVITY 0.01f

class InputController: public QObject {
    Q_OBJECT

public:

    enum ControlsSchema
    {
        CAD,
        Touchpad
    };

    InputController(std::shared_ptr<Viewport> viewport, QObject *parent = nullptr);
    virtual ~InputController();

    std::unique_ptr<OrbitalCamera> Camera;
    std::shared_ptr<Viewport> viewport = nullptr;

    bool IsKeyDown(Qt::Key key);
    void EmitCursorFromScreenEvent(QPoint screenPoint);

public:
    virtual void keyPressSlot(QKeyEvent *event);
    virtual void keyReleaseSlot(QKeyEvent *event);
    virtual void mousePressSlot(QMouseEvent *event);
    virtual void mouseReleaseSlot(QMouseEvent *event);
    virtual void mouseMoveSlot(QMouseEvent *event);
    virtual void wheelSlot(QWheelEvent *event);

signals:
    void CameraUpdated(std::shared_ptr<CameraUpdateEvent> event);
    void SceneMouseClicked(std::shared_ptr<SceneMouseClickEvent> event);
    void MoveObjectRequested(std::shared_ptr<ObjectMoveEvent> event);
    void UpdateSelectRectangle(std::shared_ptr<SelectRectangleUpdateEvent> event);

private:
    enum KeyState {
        Released,
        Pressed,
        Held
    };
    std::set<Qt::Key> knownButtons;
    std::map<Qt::Key, KeyState> keyStates;
    std::map<Qt::Key, std::unique_ptr<QTimer>> keyHeldTimers;

    QPoint lastCursorPos;
    KeyState mouseButtonStates[3];
    QPoint mouseButtonStartClick[3];
protected:

    virtual void InitlizeKeyStates();
    virtual void InitlizeMouseStates();
    int translateMouseButton(Qt::MouseButton button);

    void EmitCameraUpdateEvent();
    void EmitSceneMouseClickedEvent(QPoint screenPoint);

protected slots:
    void KeyboardKeyHeld(Qt::Key key);
};

#endif // KeyboardMouseHandlerH
