//
// Created by ksm on 4/8/22.
//

#include "Composite.h"
#include "Scene/SceneECS.h"

#pragma region CompositeElement
Composite::CompositeElement::CompositeElement(Composite *composite, std::shared_ptr<CompositeAware> el): IEntity(COMPOSITE_ELEM_CLASS)
{
    servingObjectID = el->GetAttachedObjectID();
    element = el;
    compositeTransform = composite->p_Transform;

    el->InsideComposite = true;
    dTransform = Transform::CreateRegisteredComponent(GetObjectID(), el->p_Transform->Position - compositeTransform->Position);
    dTransform->Rotation = *el->p_Transform->Rotation;
    dTransform->Scale = *el->p_Transform->Scale;
}

Composite::CompositeElement::~CompositeElement()
{
    if (auto ca = element.lock())
        ca->InsideComposite = false;
}

void Composite::CompositeElement::UpdateDTransform()
{
    if (auto ot = element.lock())
    {
        dTransform->Position = ot->p_Transform->Position - compositeTransform->Position;
        dTransform->Rotation = *ot->p_Transform->Rotation;
        dTransform->Scale = *ot->p_Transform->Scale;
    }
}

void Composite::CompositeElement::UpdateServingObject()
{
    if (auto ot = element.lock())
    {
        ot->p_Transform->DecomposeTransformations(compositeTransform->GetModelMatrix() * dTransform->GetModelMatrix());
    }
}
#pragma endregion

Composite::Composite(std::shared_ptr<CompositeAware> startObject): IEntity(COMPOSITE_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), startObject->p_Transform->Position));
    AddComponent(p_Selectable = Selectable::CreateRegisteredComponent(GetObjectID()));
    m_center = std::make_unique<Cursor>(p_Transform->Position);
    m_center->p_Transform->Scale = QVector3D(0.33f, 0.33f, 0.33f);
    objects.push_back(std::make_unique<CompositeElement>(this, startObject));

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
    selectNotiifer = p_Selectable->Selected.addNotifier([this]{
        if (!p_Selectable->Selected)
            if (auto scene = SceneECS::Instance().lock())
                scene->DestroyComposite();
    });
    
    connect(startObject.get(), &IComponent::ComponentDeleted, this, &Composite::PointFromCompositeHasBeenDeleted);
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

    p_Transform->Position.setValueBypassingBindings((objects.size() * p_Transform->Position + newObject->p_Transform->Position) / (objects.size() + 1));
    m_center->p_Transform->Position = p_Transform->Position.value();

    objects.push_back(std::make_unique<CompositeElement>(this, newObject));

    for (const std::unique_ptr<CompositeElement>& el : objects)
        el->UpdateDTransform();
    
    connect(newObject.get(), &IComponent::ComponentDeleted, this, &Composite::PointFromCompositeHasBeenDeleted);
}

void Composite::UpdateCompositeElements()
{
    m_center->p_Transform->SetPosition(p_Transform->Position.value());
    m_center->p_Transform->Rotation = p_Transform->Rotation.value();
    m_center->p_Transform->Scale = p_Transform->Scale.value() / 3;

    for (auto& obj : objects)
        obj->UpdateServingObject();
}

void Composite::PointFromCompositeHasBeenDeleted()
{
    if (auto scene = SceneECS::Instance().lock())
        scene->DestroyComposite();
}

std::vector<unsigned int> Composite::GetObjectsInside()
{
    std::vector<unsigned int> res;
    
    for (const auto& el : objects)
        res.push_back(el->servingObjectID);
    
    return res;
}


