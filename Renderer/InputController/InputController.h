#ifndef KeyboardMouseHandlerH
#define KeyboardMouseHandlerH

#include "Renderer/InputController/InputEvents/CameraUpdateEvent.h"
#include "Renderer/InputController/InputEvents/SceneMouseClickEvent.h"
#include "Renderer/Camera/Viewport.h"

#include <QKeyEvent>
#include <QObject>
#include <QPoint>
#include <set>
#include <map>

#include <Renderer/Camera/OrbitalCamera.h>

#define UNDEFINED_ID -1
#define LMOUSE_ID 0
#define MMOUSE_ID 1
#define RMOUSE_ID 2

#define MOVE_SENSITIVITY 0.005f
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

    bool IsKeyPressed(Qt::Key key);
    void EmitSceneMouseClickedEvent(QPoint screenPoint);

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

private:
    enum KeyState {
        Released,
        Pressed
    };
    std::set<Qt::Key> knownButtons;
    std::map<Qt::Key, KeyState> keyStates;

    QPoint lastCursorPos;
    KeyState mouseButtonStates[3];
protected:

    virtual void InitlizeKeyStates();
    virtual void InitlizeMouseStates();
    int translateMouseButton(Qt::MouseButton button);

    void EmitCameraUpdateEvent();
};

#endif // KeyboardMouseHandlerH
