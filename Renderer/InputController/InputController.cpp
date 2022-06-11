#include "InputController.h"
#include "Renderer/InputController/InputEvents/ObjectMoveEvent.h"



InputController::InputController(std::shared_ptr<Viewport> viewport, QObject *parent)
: QObject(parent)
{
    Camera = std::make_unique<OrbitalCamera>(QVector3D(), 5);
    this->viewport = viewport;

    InitlizeKeyStates();
    InitlizeMouseStates();
}

InputController::~InputController()
{

}

void InputController::keyPressSlot(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    Qt::Key key = (Qt::Key) event->key();
    if (knownButtons.find(key) != knownButtons.end())
    {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, key]
        {
            keyStates[key] = KeyState::Held;
            this->KeyboardKeyHeld(key);
        });
        timer->start(200);
        timer->setInterval(30);
        keyHeldTimers[key] = timer;

        keyStates[key] = KeyState::Pressed;
        //qDebug() << QString("Key %0 pressed").arg(key);
    }
}

void InputController::keyReleaseSlot(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    Qt::Key key = (Qt::Key) event->key();
    if (knownButtons.find(key) != knownButtons.end())
    {
        QTimer* timer = keyHeldTimers[key];
        keyHeldTimers.erase(key);
        timer->stop();
        delete timer;

        keyStates[key] = KeyState::Released;
        //qDebug() << QString("Key %0 released").arg(key);
    }
}

void InputController::mousePressSlot(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id == UNDEFINED_ID)
        return;

    mouseButtonStates[id] = KeyState::Pressed;
    mouseButtonStartClick[id] = event->pos();

    switch (event->button())
    {
        case Qt::LeftButton:

            break;
        case Qt::MiddleButton:

            break;
        case Qt::RightButton:
            //emit MoveObjectRequested(std::make_shared<ObjectMoveEvent>(event->pos()));
            break;
    }

    //qDebug() << QString("Mouse %0 pressed").arg(event->button());
}

void InputController::mouseReleaseSlot(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id == UNDEFINED_ID)
        return;

    switch (event->button())
    {
        case Qt::LeftButton:
            EmitSceneMouseClickedEvent(event->pos());
            break;
        case Qt::MiddleButton:
            break;
        case Qt::RightButton:
            //emit MoveObjectRequested(std::make_shared<ObjectMoveEvent>(event->pos()));
            break;
    }

    mouseButtonStates[id] = KeyState::Released;
    mouseButtonStartClick[id] = {};

    if (!(mouseButtonStates[LMOUSE_ID] == KeyState::Released or
          mouseButtonStates[MMOUSE_ID] == KeyState::Released or
          mouseButtonStates[RMOUSE_ID] == KeyState::Released))
    {
        lastCursorPos.setX(0);
        lastCursorPos.setY(0);
    }

    //qDebug() << QString("Mouse %0 released").arg(event->button());
}

void InputController::mouseMoveSlot(QMouseEvent *event)
{
    if (lastCursorPos.isNull())
    {
        lastCursorPos = event->pos();
        return;
    }
    QVector2D dMove = QVector2D(event->pos() - lastCursorPos);
    lastCursorPos = event->pos();

    if (mouseButtonStates[RMOUSE_ID] && IsKeyDown(Qt::Key_Control))
    {
        emit MoveObjectRequested(std::make_shared<ObjectMoveEvent>(event->pos()));
    }

#pragma region Camera movement
    bool cameraChanged = false;
    /*if (IsKeyPressed(Qt::Key_Shift))
    {
        if (IsKeyPressed(Qt::Key_Control))// touchpad - zoom
            Camera->ChangePivotLength(dMove.y() * ZOOM_SENSITIVITY);
        else //touchpad - pan
        {
            Camera->MoveRight(-dMove.x() * MOVE_SENSITIVITY);
            Camera->MoveUp(dMove.y() * MOVE_SENSITIVITY);
        }
        cameraChanged = true;
    }

    if (IsKeyPressed(Qt::Key_Alt)) //touchpad - rotate
    {
        Camera->RotateAroundCenter(dMove.x() * ROTATE_SENSITIVITY, dMove.y() * ROTATE_SENSITIVITY);
        cameraChanged = true;
    }*/

    if (mouseButtonStates[MMOUSE_ID]) //cad movement
    {
        if (mouseButtonStates[RMOUSE_ID])
        {
            Camera->RotateAroundCenter(dMove.x() * ROTATE_SENSITIVITY, dMove.y() * ROTATE_SENSITIVITY);
            cameraChanged = true;
        }
        else
        {
            Camera->MoveRight(-dMove.x() * MOVE_SENSITIVITY);
            Camera->MoveUp(dMove.y() * MOVE_SENSITIVITY);
            cameraChanged = true;
        }
    }

    if (cameraChanged)
        EmitCameraUpdateEvent();
#pragma endregion
}

void InputController::wheelSlot(QWheelEvent *event)
{
    Camera->ChangePivotLength(event->angleDelta().y() * ZOOM_SENSITIVITY);//tylko na touchpadzie, na myszce na odwrot

    EmitCameraUpdateEvent();
}

void InputController::InitlizeKeyStates()
{
    knownButtons.insert(Qt::Key_Shift);
    knownButtons.insert(Qt::Key_Control);
    knownButtons.insert(Qt::Key_Alt);
    knownButtons.insert(Qt::Key_W);
    knownButtons.insert(Qt::Key_S);
    knownButtons.insert(Qt::Key_A);
    knownButtons.insert(Qt::Key_D);
    knownButtons.insert(Qt::Key_Escape);

    for (Qt::Key k: knownButtons)
    {
        keyStates[k] = KeyState::Released;
    }
}

void InputController::InitlizeMouseStates()
{
    mouseButtonStates[0] = mouseButtonStates[1] = mouseButtonStates[2] = KeyState::Released;
}

int InputController::translateMouseButton(Qt::MouseButton button)
{
    switch (button)
    {
        case Qt::MouseButton::LeftButton:
            return LMOUSE_ID;
        case Qt::MouseButton::MiddleButton:
            return MMOUSE_ID;
        case Qt::MouseButton::RightButton:
            return RMOUSE_ID;
        default:
            return UNDEFINED_ID;
    }
}

#pragma region Emmiters
void InputController::EmitCameraUpdateEvent()
{
    std::shared_ptr<CameraUpdateEvent> event = std::make_shared<CameraUpdateEvent>(Camera->GetViewMatrix());
    emit CameraUpdated(event);
}

void InputController::EmitSceneMouseClickedEvent(QPoint screenPoint)
{
    emit SceneMouseClicked(std::make_shared<SceneMouseClickEvent>(screenPoint));
}

bool InputController::IsKeyDown(Qt::Key key)
{
    if (knownButtons.find(key) != knownButtons.end())
        return keyStates[key] == KeyState::Pressed or keyStates[key] == KeyState::Held ;
    return false;
}

void InputController::EmitCursorFromScreenEvent(QPoint screenPoint)
{
    emit SceneMouseClicked(std::make_shared<SceneMouseClickEvent>(screenPoint, false));
}

void InputController::KeyboardKeyHeld(Qt::Key key)
{
    bool cameraChanged = false;
    switch (key)
    {
        case Qt::Key_W:
            Camera->MoveForward(KEYBOARD_MOVE_SENSITIVITY);
            cameraChanged = true;
            break;
        case Qt::Key_S:
            Camera->MoveForward(-KEYBOARD_MOVE_SENSITIVITY);
            cameraChanged = true;
            break;
        case Qt::Key_D:
            Camera->MoveRight(KEYBOARD_MOVE_SENSITIVITY);
            cameraChanged = true;
            break;
        case Qt::Key_A:
            Camera->MoveRight(-KEYBOARD_MOVE_SENSITIVITY);
            cameraChanged = true;
            break;
    }

    if (cameraChanged)
        EmitCameraUpdateEvent();
}

#pragma endregion
