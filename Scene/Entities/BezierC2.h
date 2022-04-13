//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_BEZIERC2_H
#define SMOCAD_BEZIERC2_H


#include "IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Polyline.h"
#include "VirtualBezierC0.h"
#include "VirtualPoint.h"
#include "Scene/Components/SceneElement.h"

class BezierC2: public IEntity
{
Q_OBJECT
private slots:
    void OnDeBoorModified();
    void OnSingleBezierPointModified(QVector3D pos, unsigned int changedOID);

private:
    QPropertyNotifier selectedNotifier;
    VirtualBezierC0 m_bezier;
    Polyline m_deBoorPolyline;
    std::list<std::unique_ptr<VirtualPoint>> bezierPoints;

    void CalculateBezierPoints();

public:
    std::shared_ptr<TransformCollection> p_Collection;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    BezierC2(const QString& name);
};


#endif //SMOCAD_BEZIERC2_H
