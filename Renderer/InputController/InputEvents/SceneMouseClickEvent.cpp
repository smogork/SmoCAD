//
// Created by ksm on 3/21/22.
//

#include "SceneMouseClickEvent.h"

#include "Renderer/Renderer.h"

SceneMouseClickEvent::SceneMouseClickEvent(QPoint vPos, bool select): QEvent(User), SelectObjects(select)
{
    ViewClickPoint = vPos;
    QVector3D viewNear(ViewClickPoint.x(), Renderer::controller.viewport->GetViewportSize().height() - ViewClickPoint.y(),
                       0.0f);
    QVector3D viewFar(ViewClickPoint.x(), Renderer::controller.viewport->GetViewportSize().height() - ViewClickPoint.y(),
                      1.0f);
    QVector3D ClickViewPointNear = viewNear.unproject(Renderer::controller.Camera->GetViewMatrix(),
                                              Renderer::controller.viewport->GetProjectionMatrix(),
                                              QRect(QPoint(0.0f, 0.0f),
                                                    Renderer::controller.viewport->GetViewportSize()));
    QVector3D ClickViewPointFar = viewFar.unproject(Renderer::controller.Camera->GetViewMatrix(),
                                            Renderer::controller.viewport->GetProjectionMatrix(),
                                            QRect(QPoint(0.0f, 0.0f),
                                                  Renderer::controller.viewport->GetViewportSize()));
    PivotLength = Renderer::controller.Camera->Pivotlength();
    RaycastDirection = (ClickViewPointFar - ClickViewPointNear).toVector4D();
    RaycastStart = ClickViewPointNear.toVector4D();
    RaycastStart.setW(1.0f);

    QVector4D centerPlain = Renderer::controller.Camera->GetCenterViewPlain();
    float t = -QVector4D::dotProduct(centerPlain, RaycastStart) /
              QVector4D::dotProduct(centerPlain, RaycastDirection);
    ClickCenterPlainPoint = (RaycastDirection * t + RaycastStart).toVector3D();

    /*qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;*/
}
