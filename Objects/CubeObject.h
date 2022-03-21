//
// Created by ksm on 3/13/22.
//

#ifndef SMOCAD_CUBEOBJECT_H
#define SMOCAD_CUBEOBJECT_H


#include "TransformableObject.h"
#include "IRenderableObject.h"

#include <QOpenGLBuffer>

#include <Renderer/ShaderWrapper.h>

class CubeObject: public TransformableObject, public IRenderableObject
{
private:
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;


    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();

public:
    CubeObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader);
    ~CubeObject() override;



    int GetIndexCount() override;
    void Bind() override;
};


#endif //SMOCAD_CUBEOBJECT_H
