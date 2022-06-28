//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONCURVE_H
#define SMOCAD_INTERSECTIONCURVE_H

#include "Scene/Components/SceneElement.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Awares/IntersectionAware.h"

class IntersectionCurve : public IEntity
{
Q_OBJECT
/*private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();*/

protected:
    std::vector<QVector4D> m_paramPoints;
    std::function<QVector3D(QVector2D args)> m_sceneFunction1, m_sceneFunction2;
    bool m_cycle;

    QPropertyNotifier selectedNotifier;

    std::vector<float> GenerateGeometryVertices();
    int GetIndexCount();
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext *context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    QImage GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane);
    void FloodFill4(QPoint start, uint color, QImage& image, bool wrapX, bool wrapY);

public:
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    QColor DrawingColor = DefaultColor;

    IntersectionCurve(const QString &name, const std::vector<QVector4D> &intersectionPoints,
                      std::function<QVector3D(QVector2D args)> sceneFunction1,
                      std::function<QVector3D(QVector2D args)> sceneFunction2, bool isCycle);
    //[TODO] stworzyc oddzielny component na krzywa przeciecia!
    QImage GetTrimmingTextureOne(std::shared_ptr<IntersectionAware> plane);
    QImage GetTrimmingTextureTwo(std::shared_ptr<IntersectionAware> plane);

};

#endif //SMOCAD_INTERSECTIONCURVE_H
