//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_COMPOSITEOBJECT_H
#define SMOCAD_COMPOSITEOBJECT_H


#include "IRenderableObject.h"
#include "CursorObject.h"

class CompositeObject : public TransformableObject
{
public:
    struct CompositeTransform
    {
        CompositeTransform(CompositeObject *comp, IRenderableObject *obj)
                : dTransform( obj->Position - comp->Position, obj->Rotation, obj->Scale)
        {
            Object = obj;
        }

        CompositeTransform(const CompositeTransform &other)
                : dTransform(other.dTransform), Object(other.Object) {}

        void UpdateTransformations(CompositeObject *comp)
        {
            dTransform = TransformableObject(Object->Position - comp->Position, Object->Rotation, Object->Scale);
        }

        TransformableObject dTransform;
        IRenderableObject *Object;
    };

private:
    std::list<CompositeTransform> objects;
    std::unique_ptr<CursorObject> centerCursor = nullptr;

public:
    CompositeObject(IRenderableObject *one, IRenderableObject *two);

    ~CompositeObject();

    void AddObject(IRenderableObject *newObject);

    void ApplyTransformationsToChildren();

    std::list<CompositeTransform> &GetObjects();
    const std::unique_ptr<CursorObject> &GetCenterCursor();

    /*void Rotate(QVector3D newRot);
    void Scale(QVector3D newScale);*/
};


#endif //SMOCAD_COMPOSITEOBJECT_H
