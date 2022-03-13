//
// Created by ksm on 3/13/22.
//

#ifndef SMOCAD_CUBEOBJECT_H
#define SMOCAD_CUBEOBJECT_H


#include "TransformableObject.h"
#include "IRenderableObject.h"

class CubeObject: public TransformableObject, public IRenderableObject
{
private:
    std::vector<float> vertices;
    std::vector<int> indices;

public:
    CubeObject(QVector3D pos);

    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyEdges() override;
    unsigned int GetIndexCount() override;
};


#endif //SMOCAD_CUBEOBJECT_H
