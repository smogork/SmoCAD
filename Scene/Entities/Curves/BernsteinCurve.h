//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_BERNSTEINCURVE_H
#define SMOCAD_BERNSTEINCURVE_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Polyline.h"

//Wirtualna klasa bazowa dla krzywych
//Utworzenie jej nie ma większego sensu
class BernsteinCurve: public IEntity
{
    Q_OBJECT
protected:
    class Polyline m_bezierPolyline;

    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyIndices();
    virtual int GetIndexCount();

    virtual void InitializeDrawing();
    virtual void DrawingFunction(QOpenGLContext* context);
    virtual void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

    explicit BernsteinCurve(unsigned int cid);

protected slots:
    virtual void OnCollectionModified() { }
    virtual void OnSinglePointModified(QVector3D pos, unsigned int changedOID) { }

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QProperty<QColor> PolylineColor;
    QProperty<QColor> CurveColor;


};

#endif //SMOCAD_BERNSTEINCURVE_H
