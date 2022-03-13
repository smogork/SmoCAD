//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_IRENDERABLEOBJECT_H
#define SMOCAD_IRENDERABLEOBJECT_H

#include <vector>

/*
 * Interfejs definiujacy metody obliczające wierzcholki geometrii oraz krawedzie topologii obiektu.
 */
class IRenderableObject
{
public:
    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyEdges() = 0;
    virtual unsigned int GetIndexCount() = 0;
};


#endif //SMOCAD_IRENDERABLEOBJECT_H
