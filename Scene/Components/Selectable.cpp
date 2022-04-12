//
// Created by ksm on 4/7/22.
//

#include "Selectable.h"
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/SceneECS.h"

std::shared_ptr<Selectable> Selectable::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SelectableSystem>().lock();
        auto item = system->CreateRegistered(oid);
        item->m_transform = transform;
        return item;
    }
    return nullptr;
}

void Selectable::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SelectableSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Selectable::Selectable(unsigned int oid): IComponent(oid, SELECTABLE)
{
    m_transform = nullptr;
    m_selected = false;
}

Selectable::~Selectable()
{
    UnregisterComponent();
}

float Selectable::TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection, float cameraPivotLength)
{
    QMatrix4x4 sphereMatrix;
    //TODO: Zrobic sklaowanie przez odleglosc od kamery - punkt musi zajmowac zawsze taka sama odleglosc katowa (tw talesa)
    sphereMatrix(3,3) = -cameraPivotLength * cameraPivotLength / 600;
    QMatrix4x4 model;
    model.translate((*m_transform->Position).x(), (*m_transform->Position).y(), (*m_transform->Position).z());
    QMatrix4x4 reverseModel = model.inverted();

    sphereMatrix = reverseModel.transposed() * sphereMatrix * reverseModel;

    float a = QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastDirection);
    float b = 2 * QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastStart);
    float c = QVector4D::dotProduct(raycastStart  * sphereMatrix, raycastStart);

    float delta = b * b - 4 * a * c;
    if (delta < 0.0f)
        return NAN;

    float deltasq = std::sqrt(delta);
    float t1 = (-b + deltasq) / (2.0f * a);
    float t2 = (-b - deltasq) / (2.0f * a);

    float t = std::min(t1, t2);

    return t;
}

void Selectable::SetSelection(bool val)
{
    bool emitSignal = val != m_selected;
    m_selected = val;

    if (emitSignal)
    {
        auto e = std::make_shared<SelectionChanged>(m_selected);
        emit SelectedChanged(e);
    }
}

bool Selectable::Selected()
{
    return m_selected;
}

