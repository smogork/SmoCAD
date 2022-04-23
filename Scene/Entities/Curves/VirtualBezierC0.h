//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_VIRTUALBEZIERC0_H
#define SMOCAD_VIRTUALBEZIERC0_H


#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Polyline.h"

class VirtualBezierC0: public IEntity
{
    Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);

private:
    class Polyline m_bezierPolyline;
    QColor m_color;
    QPropertyNotifier polylineColorNotify;
    QPropertyNotifier curveColorNotify;
    QPropertyNotifier bezierPolylineDrawing;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    int GetIndexCount();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    int CalculateDrawableChunks(QMatrix4x4 proj, QMatrix4x4 view, QSize viewport);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QProperty<QColor> PolylineColor;
    QProperty<QColor> CurveColor;

    VirtualBezierC0();
};


#endif //SMOCAD_VIRTUALBEZIERC0_H
