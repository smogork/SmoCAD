//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONCURVE_H
#define SMOCAD_INTERSECTIONCURVE_H

#include "Scene/Components/SceneElement.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Awares/IntersectionAware.h"
#include "Scene/Components/IntersectionResult.h"

class IntersectionCurve : public IEntity
{
Q_OBJECT
/*private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();*/

protected:
    QPropertyNotifier selectedNotifier;

    std::vector<float> GenerateGeometryVertices();
    int GetIndexCount();
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext *context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

protected slots:
    void OnIntersectionAwareDeleted();

public:
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<IntersectionResult> p_IntersectionRes;
    QColor DrawingColor = DefaultColor;

    IntersectionCurve(const QString &name, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                      std::shared_ptr<IntersectionAware> two, bool isCycle);
    void OnComponentDeleted();
};

#endif //SMOCAD_INTERSECTIONCURVE_H
