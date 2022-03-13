//
// Created by ksm on 3/13/22.
//

#include "CubeObject.h"

CubeObject::CubeObject(QVector3D pos): TransformableObject(pos)
{
    vertices.assign({
                            +0.5f, +0.5f, +0.5f,  // front top right 0
                            +0.5f, -0.5f, +0.5f,  // front bottom right 1
                            -0.5f, -0.5f, +0.5f,  // front bottom left 2
                            -0.5f, +0.5f, +0.5f,   // front top left 3
                            +0.5f, +0.5f, -0.5f,  // back top right 4
                            +0.5f, -0.5f, -0.5f,  // back bottom right 5
                            -0.5f, -0.5f, -0.5f,  // back bottom left 6
                            -0.5f, +0.5f, -0.5f  //back top left 7
                    });

    indices.assign({
                           0, 1,
                           1, 2,
                           2, 3,
                           0, 3,
                           4, 5,
                           5, 6,
                           6, 7,
                           4, 7,
                           3, 7,
                           2, 6,
                           1, 5,
                           0, 4,
                   });
}

std::vector<float> CubeObject::GenerateGeometryVertices()
{
    return vertices;
}

std::vector<int> CubeObject::GenerateTopologyEdges()
{
    return indices;
}

unsigned int CubeObject::GetIndexCount()
{
    return indices.size();
}
