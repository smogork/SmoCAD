//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_COMPOSITEOBJECT_H
#define SMOCAD_COMPOSITEOBJECT_H


#include "IRenderableObject.h"

class CompositeObject: public TransformableObject
{
public:
    struct CompositeTransform{
        CompositeTransform(CompositeObject* comp, IRenderableObject* obj)
        : dTransform(comp->Position - obj->Position,
                     comp->Rotation - obj->Rotation,
                     QVector3D(1.0f, 1.0f, 1.0f))
        {
            Object = obj;
        }
        CompositeTransform(const CompositeTransform& other)
        : dTransform(other.dTransform), Object(other.Object) { }

        void UpdateTransformations(CompositeObject* comp)
        {
            dTransform = TransformableObject(
                    comp->Position - Object->Position,
                    comp->Rotation - Object->Rotation,
                    QVector3D(1.0f, 1.0f, 1.0f)
            );
        }

        TransformableObject dTransform;
        IRenderableObject* Object;
    };

private:
    std::list<CompositeTransform> objects;
    //TransformableObject compositeTransformations;

public:
    CompositeObject(IRenderableObject* one, IRenderableObject* two);
    ~CompositeObject();

    void AddObject(IRenderableObject* newObject);
    void ApplyTransformationsToChildren();
    std::list<CompositeTransform> &GetObjects();

    /*void Rotate(QVector3D newRot);
    void Scale(QVector3D newScale);*/
};


#endif //SMOCAD_COMPOSITEOBJECT_H
