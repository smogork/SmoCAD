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
    virtual void Bind(ShaderWrapper* shader)
    {
        if (Selected)
            shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
        else
            shader->SetUniform("u_ObjectColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));

        shader->Bind();
        va->bind();
    }

    virtual void Release(ShaderWrapper* shader) { va->release(); shader->Release(); }

};


#endif //SMOCAD_IRENDERABLEOBJECT_H
