//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_BASECURVE_H
#define SMOCAD_BASECURVE_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Polyline.h"
#include "Scene/Entities/Points/InvisiblePoint.h"
#include "Models/Curves/Bezier.h"

//Wirtualna klasa bazowa dla krzywych
//Utworzenie jej nie ma większego sensu
class BaseCurve: public IEntity
{
    Q_OBJECT
protected:
    class Polyline m_curvePolyline;
    QPropertyNotifier curvePolylineDrawing;
    QPropertyNotifier curvePolylineColor;

    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyIndices() = 0;
    virtual int GetIndexCount() = 0;

    virtual void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

    void CommonSerializeFunction(MG1::Bezier& b);
    void CommonDeserializeFunction(const MG1::Bezier& b);

    explicit BaseCurve(unsigned int cid);
    explicit BaseCurve(unsigned int cid, uint explicit_oid);
    void InitObject();

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QProperty<QColor> PolylineColor;
    QProperty<QColor> CurveColor;

};

#endif //SMOCAD_BASECURVE_H
