//
// Created by ksm on 4/8/22.
//

#include "Composite.h"

#pragma region CompositeElement
Composite::CompositeElement::CompositeElement(Composite *composite, std::shared_ptr<Transform> objTransform,
                                              std::shared_ptr<Drawing> objDrawing): IEntity(COMPOSITE_ELEM_CLASS)
{
    servingObjectID = objTransform->GetAttachedObjectID();
    this->objDrawing = objDrawing;
    this->objTransform = objTransform;
    originalUniform = objDrawing->p_uniformFunction;
    this->objDrawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&CompositeElement::DecoratingUniformFunction);
    compositeTransform = composite->p_Transform;

    dTransform = Transform::CreateRegisteredComponent(objectID, objTransform->Position - compositeTransform->Position);
    dTransform->Rotation = objTransform->Rotation;
    dTransform->Scale = objTransform->Scale;
}

Composite::CompositeElement::~CompositeElement()
{
    objTransform->DecomposeTransformations(dTransform->GetModelMatrix() * compositeTransform->GetModelMatrix());
    objDrawing->p_uniformFunction = originalUniform;
}

void Composite::CompositeElement::DecoratingUniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    if (originalUniform)
        originalUniform(shader);

    shader->SetUniform("u_MVP.Model", dTransform->GetModelMatrix() * compositeTransform->GetModelMatrix());
}
#pragma endregion

Composite::Composite(std::shared_ptr<CompositeAware> startObject): IEntity(COMPOSITE_CLASS)
{
    p_Transform = Transform::CreateRegisteredComponent(objectID, startObject->p_Transform->Position);
    m_center = std::make_unique<Cursor>(p_Transform->Position);
    objects.push_back(std::make_unique<CompositeElement>(this, startObject->p_Transform, startObject->p_Drawing));
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

    SetPosition((objects.size() * p_Transform->Position + newObject->p_Transform->Position) / (objects.size() + 1));
    objects.push_back(std::make_unique<CompositeElement>(this, newObject->p_Transform, newObject->p_Drawing));
}

void Composite::SetPosition(QVector3D pos)
{
    p_Transform->Position = pos;
    m_center->p_Transform->Position = pos;
}


