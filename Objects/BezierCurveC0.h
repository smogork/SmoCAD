//
// Created by ksm on 3/27/22.
//

#ifndef SMOCAD_BEZIERCURVEC0_H
#define SMOCAD_BEZIERCURVEC0_H


#include "IRenderableObject.h"
#include "PointObject.h"
#include "Scene/Events/PointObjectChangedEvent.h"

class BezierCurveC0: public IRenderableObject, public QObject
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

public slots:
    void onPointChanged(std::shared_ptr<PointObjectChangedEvent> event);
};


#endif //SMOCAD_BEZIERCURVEC0_H
