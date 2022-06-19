//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONCURVE_H
#define SMOCAD_INTERSECTIONCURVE_H

#include "Scene/Components/SceneElement.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Entities/IEntity.h"

class IntersectionCurve: public IEntity
{
    Q_OBJECT
/*private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();*/
    
protected:
    int m_pointCount;
    
    std::vector<float> GenerateGeometryVertices( const std::vector<QVector3D>& points);
    void InitializeDrawing( const std::vector<QVector3D>& points);
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    
public:
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    QColor DrawingColor = DefaultColor;
    
    IntersectionCurve(const QString& name, const std::vector<QVector3D>& intersectionPoints);
    
};

#endif //SMOCAD_INTERSECTIONCURVE_H
