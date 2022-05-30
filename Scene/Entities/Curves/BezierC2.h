//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_BEZIERC2_H
#define SMOCAD_BEZIERC2_H


#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Entities/Polyline.h"
#include "VirtualBezierC0.h"
#include "Scene/Entities/Points/VirtualPoint.h"
#include "Scene/Components/SceneElement.h"
#include "BaseCurve.h"

class BezierC2: public BaseCurve
{
Q_OBJECT
private slots:
    void OnDeBoorModified();
    void OnSingleBezierPointModified(QVector3D pos, unsigned int changedOID);

private:
    QPropertyNotifier selectedNotifier;
    QPropertyNotifier deBoorPolylineDrawing;
    class Polyline m_deBoorPolyline;
    std::list<std::unique_ptr<VirtualPoint>> bezierPoints;

    void CalculateBezierPoints();
    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyIndices() override;
    int GetIndexCount() override;

    void InitializeDrawing() override;

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit BezierC2(const QString& name);
};


#endif //SMOCAD_BEZIERC2_H
