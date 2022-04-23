//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_BERNSTEINCURVE_H
#define SMOCAD_BERNSTEINCURVE_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Polyline.h"
#include "Scene/Entities/Points/InvisiblePoint.h"

//Wirtualna klasa bazowa dla krzywych
//Utworzenie jej nie ma większego sensu
class BernsteinCurve: public IEntity
{
    Q_OBJECT
protected:
    class Polyline m_bezierPolyline;
    QPropertyNotifier bezierPolylineDrawing;

    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyIndices() = 0;
    virtual int GetIndexCount() = 0;

    virtual void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

    explicit BernsteinCurve(unsigned int cid);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QProperty<QColor> PolylineColor;
    QProperty<QColor> CurveColor;


};

#endif //SMOCAD_BERNSTEINCURVE_H
