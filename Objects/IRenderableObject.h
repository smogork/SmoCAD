//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_IRENDERABLEOBJECT_H
#define SMOCAD_IRENDERABLEOBJECT_H

#include <QOpenGLVertexArrayObject>
#include <vector>

#include <Renderer/ShaderWrapper.h>
#include "TransformableObject.h"

/*
 * Interfejs definiujacy metody obliczające wierzcholki geometrii oraz krawedzie topologii obiektu.
 */
class IRenderableObject: public TransformableObject
{
private:
    bool buffersCreated;

protected:
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;

public:
    bool Selected = false;

    IRenderableObject(QVector3D pos):
            TransformableObject(pos)
    {
        va = std::make_unique<QOpenGLVertexArrayObject>();
    }
    virtual ~IRenderableObject()
    {
        if (va->isCreated())
            va->destroy();
    }

    bool AreBuffersCreated() { return buffersCreated; }

    virtual void DefineBuffers() { buffersCreated = true; }
    virtual int GetIndexCount() = 0;
    virtual int GetDrawType() = 0;
    virtual float TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection) { return NAN; }
    virtual void Bind(ShaderWrapper* shader)
    {
        shader->Bind();
        va->bind();
    }
    virtual void Release(ShaderWrapper* shader) { va->release(); shader->Release(); }

};


#endif //SMOCAD_IRENDERABLEOBJECT_H
