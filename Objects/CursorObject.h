//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_CURSOROBJECT_H
#define SMOCAD_CURSOROBJECT_H


#include <QOpenGLBuffer>
#include "IRenderableObject.h"
#include "TransformableObject.h"

class CursorObject: public IRenderableObject
{
private:
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();

public:
    CursorObject(QVector3D pos);
    ~CursorObject() override;

    void DefineBuffers() override;
    int GetIndexCount() override;
    int GetDrawType() override { return GL_LINES; }
    void Bind(ShaderWrapper* shader) override;
};


#endif //SMOCAD_CURSOROBJECT_H
