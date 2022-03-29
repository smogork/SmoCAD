#include "InputController.h"


InputController::InputController(std::shared_ptr<Viewport> viewport, QObject *parent)
: QObject(parent)
{
    this->viewport = viewport;
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
    Qt::Key key = (Qt::Key) event->key();
    if (knownButtons.find(key) != knownButtons.end())
        keyStates[key] = KeyState::Pressed;
}

void InputController::keyReleaseSlot(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key) event->key();
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

    switch (event->button())
    {
        case Qt::LeftButton:
            EmitSceneMouseClickedEvent(event->pos(), true);
        case Qt::RightButton:
            EmitSceneMouseClickedEvent(event->pos(), false);
    }

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

    if (IsKeyPressed(Qt::Key_Shift))
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

void InputController::EmitSceneMouseClickedEvent(QPoint screenPoint, bool unselect)
{
    QVector3D viewNear(screenPoint.x(), viewport->GetViewportSize().height() - screenPoint.y(), 0.0f);
    QVector3D viewFar(screenPoint.x(), viewport->GetViewportSize().height() - screenPoint.y(), 1.0f);
    QVector3D resultNear = viewNear.unproject(Camera->GetViewMatrix(), viewport->GetProjectionMatrix(), QRect(QPoint(0.0f, 0.0f), viewport->GetViewportSize()));
    QVector3D resultFar = viewFar.unproject(Camera->GetViewMatrix(), viewport->GetProjectionMatrix(), QRect(QPoint(0.0f, 0.0f), viewport->GetViewportSize()));

    /*qDebug() << "Camera position:" << Camera->GetPosition();
    qDebug() << "Camera center position:" << Camera->CenterPoint;
    qDebug() << "UnprojectNear:" << resultNear;
    qDebug() << "ViewNear:" << viewNear;
    qDebug() << "UnprojectFar:" << resultFar;
    qDebug() << "ViewFar:" << viewFar;*/

    std::shared_ptr<SceneMouseClickEvent> event = std::make_shared<SceneMouseClickEvent>(screenPoint, resultNear, resultFar, unselect);
    emit SceneMouseClicked(event);
}

bool InputController::IsKeyPressed(Qt::Key key)
{
    return keyStates[key] == KeyState::Pressed;
}


#pragma endregion
