#ifndef KeyboardMouseHandlerH
#define KeyboardMouseHandlerH

#include "cameramovementevent.h"

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

    InputController(QObject *parent = nullptr);
    virtual ~InputController();

    std::unique_ptr<OrbitalCamera> Camera;

public:
    virtual void keyPressSlot(QKeyEvent *event);
    virtual void keyReleaseSlot(QKeyEvent *event);
    virtual void mousePressSlot(QMouseEvent *event);
    virtual void mouseReleaseSlot(QMouseEvent *event);
    virtual void mouseMoveSlot(QMouseEvent *event);
    virtual void wheelSlot(QWheelEvent *event);

signals:
    void CameraUpdated(std::shared_ptr<CameraMovementEvent> event);

private:
    enum KeyState {
        Pressed,
        Released
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
