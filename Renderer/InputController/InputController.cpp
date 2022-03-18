#include "InputController.h"


InputController::InputController(QObject *parent)
{
    //[TODO] poprawic tworzenie kamery aby patrzyla na jakis sensowny obiekt
    Camera = std::make_unique<OrbitalCamera>(QVector3D(), 5);

    InitlizeKeyStates();
    InitlizeMouseStates();
}

InputController::~InputController()
{

}

void InputController::keyPressSlot(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key)event->key();
    if (knownButtons.find(key) != knownButtons.end())
        keyStates[key] = KeyState::Pressed;
}
void InputController::keyReleaseSlot(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key)event->key();
    if (knownButtons.find(key) != knownButtons.end())
        keyStates[key] = KeyState::Released;
}

void InputController::mousePressSlot(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id != UNDEFINED_ID)
        mouseButtonStates[id] = KeyState::Pressed;
}

void InputController::mouseReleaseSlot(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id != UNDEFINED_ID)
        mouseButtonStates[id] = KeyState::Released;

    if (!(mouseButtonStates[LMOUSE_ID] == KeyState::Released or
          mouseButtonStates[MMOUSE_ID] == KeyState::Released or
          mouseButtonStates[RMOUSE_ID] == KeyState::Released))
    {
        lastCursorPos.setX(0);
        lastCursorPos.setY(0);
    }
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

    bool cameraChanged = false;

    if (keyStates[Qt::Key_Shift] == KeyState::Pressed)
    {
        if (keyStates[Qt::Key_Control] == KeyState::Pressed)// touchpad - zoom
        {
            Camera->ChangePivotLength(dMove.y() * ZOOM_SENSITIVITY);
        }
        else //touchpad - pan
        {
            Camera->MoveRight(-dMove.x() * MOVE_SENSITIVITY);
            Camera->MoveUp(dMove.y() * MOVE_SENSITIVITY);
        }
        cameraChanged = true;
    }

    if (keyStates[Qt::Key_Alt] == KeyState::Pressed) //touchpad - rotate
    {
        Camera->RotateAroundCenter(dMove.x() * ROTATE_SENSITIVITY, dMove.y() * ROTATE_SENSITIVITY);
        cameraChanged = true;
    }

    /*if (mousePresses[MMOUSE_ID]) //cad movement
    {
        if (mousePresses[RMOUSE_ID])
        {
            camera->RotateAroundCenter(dMove.x() * ROTATE_SENSITIVITY, dMove.y() * ROTATE_SENSITIVITY);
        }
        else
        {
            camera->MoveRight(-dMove.x() * MOVE_SENSITIVITY);
            camera->MoveUp(dMove.y() * MOVE_SENSITIVITY);
        }

        makeCurrent ();
        shader->bind();
        int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
        shader->setUniformValue(u_viewMatrixLoc, camera->GetViewMatrix());
        update();
    }*/

    if (cameraChanged)
        EmitCameraUpdateEvent();
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

    for (Qt::Key k : knownButtons)
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
    switch(button)
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

void InputController::EmitCameraUpdateEvent()
{
    std::shared_ptr<CameraMovementEvent> event = std::make_shared<CameraMovementEvent>(Camera->GetViewMatrix());
    emit CameraUpdated(event);
}
