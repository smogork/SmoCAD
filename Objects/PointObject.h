//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_POINTOBJECT_H
#define SMOCAD_POINTOBJECT_H


#include "CubeObject.h"

class PointObject: public IRenderableObject
{
private:
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();

public:
    PointObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader);
    ~PointObject() override;

    int GetIndexCount() override;
    int GetDrawType() override { return GL_POINTS; }
    void Bind() override;
};

#endif //SMOCAD_POINTOBJECT_H
