//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_IRENDERABLEOBJECT_H
#define SMOCAD_IRENDERABLEOBJECT_H

/*
 * Interfejs definiujacy metody obliczające wierzcholki geometrii oraz krawedzie topologii obiektu.
 */
class IRenderableObject
{
public:
    virtual void GenerateGeometryVertices() = 0;
    virtual void GenerateTopologyEdges() = 0;
};


#endif //SMOCAD_IRENDERABLEOBJECT_H
