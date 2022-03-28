//
// Created by ksm on 3/27/22.
//

#ifndef SMOCAD_BEZIERCURVEC0_H
#define SMOCAD_BEZIERCURVEC0_H


#include "IRenderableObject.h"
#include "PointObject.h"

class BezierCurveC0: public IRenderableObject
{
private:
    std::list<PointObject*> controlPoints;
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyEdges();
    void CreateBuffers();

public:
    explicit BezierCurveC0();
    ~BezierCurveC0() override;

    void DefineBuffers() override;
    void UpdateBuffers() override;
    int GetIndexCount() override;
    int GetDrawType() override { return GL_PATCHES; }
    void Bind(ShaderWrapper* shader) override;

    void AddControlPoint(PointObject* point);
    void RemovePoint(PointObject* point);
};


#endif //SMOCAD_BEZIERCURVEC0_H
