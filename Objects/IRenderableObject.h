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

    //[TODO] przerobic aby obiekty zwracaly jakies ludzkie typu na punkty i krawedzie
    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyEdges() = 0;
    virtual int GetIndexCount() = 0;
};


#endif //SMOCAD_IRENDERABLEOBJECT_H
