//
// Created by ksm on 5/14/22.
//

#ifndef SMOCAD_PLAINC0_H
#define SMOCAD_PLAINC0_H

#include <QPropertyNotifier>
#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Polyline.h"

class PlainC0: public IEntity
{
    Q_OBJECT
protected:
    class Polyline m_polyline;
    QPropertyNotifier bezierPolylineDrawing;
    QPropertyNotifier bezierPolylineColor;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    int GetIndexCount();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

    explicit PlainC0(unsigned int cid);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QProperty<QColor> PolylineColor;
    QProperty<QColor> CurveColor;


};

#endif //SMOCAD_PLAINC0_H
