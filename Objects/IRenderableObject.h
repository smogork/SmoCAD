//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_IRENDERABLEOBJECT_H
#define SMOCAD_IRENDERABLEOBJECT_H

#include <QOpenGLVertexArrayObject>
#include <vector>

/*
 * Interfejs definiujacy metody obliczające wierzcholki geometrii oraz krawedzie topologii obiektu.
 */
class IRenderableObject
{
protected:
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;

public:
    IRenderableObject()
    {
        va = std::make_unique<QOpenGLVertexArrayObject>();
    }
    virtual ~IRenderableObject()
    {
        if (va->isCreated())
            va->destroy();
    }

    //[TODO] przerobic aby obiekty zwracaly jakies ludzkie typu na punkty i krawedzie
    //virtual std::vector<float> GenerateGeometryVertices() = 0;
    //virtual std::vector<int> GenerateTopologyEdges() = 0;

    virtual int GetIndexCount() = 0;
    virtual void BindVertexArray() { va->bind(); }
    virtual void ReleaseVertexArray() { va->release(); }
};


#endif //SMOCAD_IRENDERABLEOBJECT_H
