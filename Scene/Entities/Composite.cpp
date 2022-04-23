//
// Created by ksm on 4/8/22.
//

#include "Composite.h"

#pragma region CompositeElement
Composite::CompositeElement::CompositeElement(Composite *composite, std::shared_ptr<Transform> objTransform): IEntity(COMPOSITE_ELEM_CLASS)
{
    servingObjectID = objTransform->GetAttachedObjectID();
    this->objTransform = objTransform;
    compositeTransform = composite->p_Transform;

    dTransform = Transform::CreateRegisteredComponent(objectID, objTransform->Position - compositeTransform->Position);
    dTransform->Rotation = *objTransform->Rotation;
    dTransform->Scale = *objTransform->Scale;
}

Composite::CompositeElement::~CompositeElement()
{

}

void Composite::CompositeElement::UpdateDTransform()
{
    if (auto ot = objTransform.lock())
    {
        dTransform->Position = ot->Position - compositeTransform->Position;
        dTransform->Rotation = *ot->Rotation;
        dTransform->Scale = *ot->Scale;
    }
}

void Composite::CompositeElement::UpdateServingObject()
{
    if (auto ot = objTransform.lock())
    {
        ot->DecomposeTransformations(compositeTransform->GetModelMatrix() * dTransform->GetModelMatrix());
    }
}
#pragma endregion

Composite::Composite(std::shared_ptr<CompositeAware> startObject): IEntity(COMPOSITE_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(objectID, startObject->p_Transform->Position));
    AddComponent(p_Selectable = Selectable::CreateRegisteredComponent(objectID));
    m_center = std::make_unique<Cursor>(p_Transform->Position);
    m_center->p_Transform->Scale = QVector3D(0.33f, 0.33f, 0.33f);
    objects.push_back(std::make_unique<CompositeElement>(this, startObject->p_Transform));

    compositePositionNotifier = p_Transform->Position.addNotifier([this]()
        {
            UpdateCompositeElements();
        });
    compositeRotationNotifier = p_Transform->Rotation.addNotifier([this]()
        {
          UpdateCompositeElements();
        });
    compositeScaleNotifier = p_Transform->Scale.addNotifier([this]()
        {
          UpdateCompositeElements();
        });
}

Composite::~Composite()
{
    objects.clear();
}

void Composite::AddObject(std::shared_ptr<CompositeAware> newObject)
{
    for (const std::unique_ptr<CompositeElement>& el : objects)
        if (el->servingObjectID == newObject->GetAttachedObjectID())
            return;

    newObje

    p_Transform->Position.setValueBypassingBindings((objects.size() * p_Transform->Position + newObject->p_Transform->Position) / (objects.size() + 1));
    m_center->p_Transform->Position = p_Transform->Position.value();

    objects.push_back(std::make_unique<CompositeElement>(this, newObject->p_Transform));

    for (const std::unique_ptr<CompositeElement>& el : objects)
        el->UpdateDTransform();
}

void Composite::UpdateCompositeElements()
{
    m_center->p_Transform->Position = p_Transform->Position.value();
    m_center->p_Transform->Rotation = p_Transform->Rotation.value();
    m_center->p_Transform->Scale = p_Transform->Scale.value() / 3;

    for (auto& obj : objects)
        obj->UpdateServingObject();
}


