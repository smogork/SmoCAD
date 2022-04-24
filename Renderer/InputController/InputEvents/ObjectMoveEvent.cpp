//
// Created by ksm on 4/23/22.
//

#include "ObjectMoveEvent.h"
#include "Renderer/Renderer.h"

ObjectMoveEvent::ObjectMoveEvent(QPoint screenPoint): QEvent(User)
{
    QVector3D viewNear(screenPoint.x(), Renderer::controller.viewport->GetViewportSize().height() - screenPoint.y(),
                       0.0f);
    QVector3D viewFar(screenPoint.x(), Renderer::controller.viewport->GetViewportSize().height() - screenPoint.y(),
                      1.0f);
    QVector3D resultNear = viewNear.unproject(Renderer::controller.Camera->GetViewMatrix(),
                                              Renderer::controller.viewport->GetProjectionMatrix(),
                                              QRect(QPoint(0.0f, 0.0f),
                                              Renderer::controller.viewport->GetViewportSize()));
    QVector3D resultFar = viewFar.unproject(Renderer::controller.Camera->GetViewMatrix(),
                                            Renderer::controller.viewport->GetProjectionMatrix(),
                                            QRect(QPoint(0.0f, 0.0f),
                                            Renderer::controller.viewport->GetViewportSize()));

    RaycastDirection = (resultFar - resultNear).toVector4D();
    RaycastStart = resultNear.toVector4D();
    RaycastStart.setW(1.0f);
    CameraBackVersor = Renderer::controller.Camera->GetPivotVector();
}
