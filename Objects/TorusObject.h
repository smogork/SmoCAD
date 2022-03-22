//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_TORUSOBJECT_H
#define SMOCAD_TORUSOBJECT_H


#include <QOpenGLBuffer>
#include "TransformableObject.h"
#include "IRenderableObject.h"

class TorusObject: public IRenderableObject
{
private:
    float biggerR;
    float smallerR;
    int biggerRDensity;
    int smallerRDensity;

    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();
    void UpdateBuffers();
    bool buffersCreated = false;
public:

    TorusObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader, float R, float r, int RDensity, int rDensity);
    ~TorusObject() override;

    void SetBiggerRadius(float value);
    void SetSmallerRadius(float value);
    void SetBiggerRadiusDensity(int value);
    void SetSmallerRadiusDensity(int value);

    float GetBiggerR();
    float GetSmallerR();
    int GetBiggerRDensity();
    int GetSmallerRDensity();

    int GetIndexCount() override;
    int GetDrawType() override { return GL_LINES; }
    void Bind() override;
};


#endif //SMOCAD_TORUSOBJECT_H
