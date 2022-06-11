//
// Created by ksm on 4/13/22.
//

#include "ScreenSelectable.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/ScreenSelectableSystem.h"
#include "Mathematics/GeometryRelation.h"

std::shared_ptr<ScreenSelectable>
ScreenSelectable::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform,
                                            std::shared_ptr<Selectable> select,
                                            std::shared_ptr<CompositeAware> composite)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<ScreenSelectableSystem>().lock();
        auto item = system->CreateRegistered(oid);
        item->p_Selected = select;
        item->m_transform = transform;
        item->p_CompositeAware = composite;
        return item;
    }
    return nullptr;
}

void ScreenSelectable::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<ScreenSelectableSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

ScreenSelectable::ScreenSelectable(unsigned int oid) : IComponent(oid, SCREEN_SELECTABLE)
{

}

ScreenSelectable::~ScreenSelectable()
{
    UnregisterComponent();
}

float ScreenSelectable::TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection, float cameraPivotLength)
{
    QMatrix4x4 sphereMatrix;
    //TODO: Zrobic sklaowanie przez odleglosc od kamery - punkt musi zajmowac zawsze taka sama odleglosc katowa (tw talesa)
    sphereMatrix(3, 3) = -cameraPivotLength * cameraPivotLength / 600;
    QMatrix4x4 model;
    model.translate((*m_transform->Position).x(), (*m_transform->Position).y(), (*m_transform->Position).z());
    QMatrix4x4 reverseModel = model.inverted();
    
    sphereMatrix = reverseModel.transposed() * sphereMatrix * reverseModel;
    
    float a = QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastDirection);
    float b = 2 * QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastStart);
    float c = QVector4D::dotProduct(raycastStart * sphereMatrix, raycastStart);
    
    float delta = b * b - 4 * a * c;
    if (delta < 0.0f)
        return NAN;
    
    float deltasq = std::sqrt(delta);
    float t1 = (-b + deltasq) / (2.0f * a);
    float t2 = (-b - deltasq) / (2.0f * a);
    
    float t = std::min(t1, t2);
    
    return t;
}

bool ScreenSelectable::IsInsideRectangle(QRect selectRectangle)
{
    QVector3D viewPos = m_transform->Position.value()
            .project(Renderer::controller.Camera->GetViewMatrix(), Renderer::controller.viewport->GetProjectionMatrix(),
                     QRect(QPoint(0.0f, 0.0f), Renderer::controller.viewport->GetViewportSize()));
    
    return GeometryRelation::IsPointInsideRect(
            QPoint(viewPos.x(), Renderer::controller.viewport->GetViewportSize().height() - viewPos.y()),
            selectRectangle);
}

bool ScreenSelectable::CanBeInsideComposite()
{
    return p_CompositeAware != nullptr;
}
