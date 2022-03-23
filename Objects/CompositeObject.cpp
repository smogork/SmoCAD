//
// Created by ksm on 3/22/22.
//

#include "CompositeObject.h"

CompositeObject::CompositeObject(IRenderableObject* one, IRenderableObject* two):
    TransformableObject((one->Position + two->Position) / 2)
{
    centerCursor = std::make_unique<CursorObject>(Position);

    one->Selected = true;
    two->Selected = true;
    objects.push_back(CompositeTransform(this, one));
    objects.push_back(CompositeTransform(this, two));
}

CompositeObject::~CompositeObject()
{

}

void CompositeObject::AddObject(IRenderableObject* newObject)
{
    for (CompositeTransform& obj : objects)
        if (obj.Object == newObject)
            return;

    Position = (objects.size() * Position + newObject->Position) / (objects.size() + 1);

    for (CompositeTransform& obj : objects)
        obj.UpdateTransformations(this);
    objects.push_back(CompositeTransform(this, newObject));
    newObject->Selected = true;
}

void CompositeObject::ApplyTransformationsToChildren()
{
    for (CompositeTransform& obj : objects)
    {
        TransformableObject transformations = TransformableObject::DecomposeTransformations(
                GetModelMatrix() * obj.dTransform.GetModelMatrix());

        obj.Object->Position = transformations.Position;
        obj.Object->Rotation = transformations.Rotation;
        obj.Object->Scale = transformations.Scale;
        obj.Object->Selected = false;

    }
}

std::list<CompositeObject::CompositeTransform> &CompositeObject::GetObjects()
{
    return objects;
}

const std::unique_ptr<CursorObject> &CompositeObject::GetCenterCursor()
{
    centerCursor->Position = Position;
    centerCursor->Rotation = Rotation;
    centerCursor->Scale = Scale;

    return centerCursor;
}
