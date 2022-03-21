//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_IRENDERABLEOBJECT_H
#define SMOCAD_IRENDERABLEOBJECT_H

#include <QOpenGLVertexArrayObject>
#include <vector>

#include <Renderer/ShaderWrapper.h>

/*
 * Interfejs definiujacy metody obliczające wierzcholki geometrii oraz krawedzie topologii obiektu.
 */
class IRenderableObject
{
protected:
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;

public:
    std::shared_ptr<ShaderWrapper> Shader = nullptr;

    IRenderableObject(std::shared_ptr<ShaderWrapper> shader):
        Shader(shader)
    {
        va = std::make_unique<QOpenGLVertexArrayObject>();
    }
    virtual ~IRenderableObject()
    {
        if (va->isCreated())
            va->destroy();
    }

    virtual int GetIndexCount() = 0;
    virtual void Bind() { Shader->Bind(); va->bind(); }
    virtual void Release() { va->release(); Shader->Release(); }
};


#endif //SMOCAD_IRENDERABLEOBJECT_H
