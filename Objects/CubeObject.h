//
// Created by ksm on 3/13/22.
//

#ifndef SMOCAD_CUBEOBJECT_H
#define SMOCAD_CUBEOBJECT_H


#include "TransformableObject.h"
#include "IRenderableObject.h"

#include <QOpenGLBuffer>

#include <Renderer/ShaderWrapper.h>

class CubeObject: public IRenderableObject
{
private:
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();

public:
    explicit CubeObject(QVector3D pos);
    ~CubeObject() override;

    void DefineBuffers() override;
    int GetIndexCount() override;
    int GetDrawType() override { return GL_LINES; }
    void Bind(ShaderWrapper* shader) override;
};


#endif //SMOCAD_CUBEOBJECT_H
