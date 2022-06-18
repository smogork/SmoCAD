//
// Created by ksm on 4/7/22.
//

#include <cfloat>
#include "SelectableSystem.h"
#include "Scene/SceneECS.h"
#include "TransformSystem.h"

SelectableSystem::SelectableSystem(): ISystem(SYSTEM_ID::SELECTABLE)
{
    QObject::connect(&Renderer::controller, &InputController::MoveObjectRequested,
                     this, &SelectableSystem::OnSelectedMoveRequest);
    QObject::connect(&Renderer::controller, &InputController::RemoveSelection,
                     this, &SelectableSystem::OnRemoveSelection);
}


void SelectableSystem::ClearSystem()
{
    selectedObject = nullptr;
}

std::shared_ptr<Selectable> SelectableSystem::CreateRegistered(unsigned int oid)
{
    auto item = ISystem::CreateRegistered(oid);

    std::weak_ptr<Selectable> weakItem = item;
    auto notifier = item->Selected.addNotifier([this,weakItem](){
        if (auto s = weakItem.lock())
        {
            if (s->Selected)
            {
                if (this->selectedObject)
                    this->selectedObject->Selected = false;
                selectedObject = s;
            }
        }
    });
    notifiers.insert(std::make_pair(item->GetAttachedObjectID(), std::move(notifier)));

    return item;
}

bool SelectableSystem::Unregister(unsigned int oid)
{
    notifiers.erase(oid);
    return ISystem::Unregister(oid);
}

bool SelectableSystem::RegisterComponent(std::shared_ptr<Selectable> component)
{
    bool res = ISystem::RegisterComponent(component);
    if (res)
    {
        std::weak_ptr<Selectable> weakItem = component;
        auto notifier = component->Selected.addNotifier([this,weakItem](){
            if (auto s = weakItem.lock())
            {
                if (s->Selected)
                {
                    if (this->selectedObject)
                        this->selectedObject->Selected = false;
                    selectedObject = s;
                }
            }
        });
    }
    return res;
}

void SelectableSystem::Unselect()
{
    if (selectedObject)
    {
        this->selectedObject->Selected = false;
        this->selectedObject.reset();
    }
}

void SelectableSystem::OnSelectedMoveRequest(std::shared_ptr<ObjectMoveEvent> event)
{
    //Czy aktualnie zaznaczony obiekt posiada Transform?
    if (!selectedObject)
        return;

    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto selectedTransform = scene->GetComponentOfSystem<TransformSystem, Transform>(selectedObject->GetAttachedObjectID()).lock())
        {
            //Wylicz płaszczyzne równoległą do ekranu przechodzącą przez poruszany obiekt
            QVector4D objPlain = event->CameraBackVersor.toVector4D();
            objPlain.setW(QVector3D::dotProduct(event->CameraBackVersor, -selectedTransform->Position));

            //Rowiąż równanie raycastingu do prostej wysłanej z ekranu aby wyznaczyć nowe położenie obiektu
            float t = -QVector4D::dotProduct(objPlain, event->RaycastStart) /
                      QVector4D::dotProduct(objPlain, event->RaycastDirection);
            selectedTransform->SetPosition((event->RaycastDirection * t + event->RaycastStart).toVector3D());
        }
    }
}

void SelectableSystem::OnRemoveSelection()
{
    Unselect();
}


