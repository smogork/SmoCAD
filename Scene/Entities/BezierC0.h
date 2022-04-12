//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_BEZIERC0_H
#define SMOCAD_BEZIERC0_H


#include "IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Polyline.h"

class BezierC0: public IEntity
{
    Q_OBJECT
private slots:
    void OnCollectionModified();

private:
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    int GetIndexCount();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    int CalculateDrawableChunks(QMatrix4x4 proj, QMatrix4x4 view, QSize viewport);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    Polyline BezierPolyline;

    BezierC0();
};


#endif //SMOCAD_BEZIERC0_H
