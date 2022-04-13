//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_CUBE_H
#define SMOCAD_CUBE_H


#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"
#include "Scene/Components/CompositeAware.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"

class Cube: public IEntity
{
private:
    static std::vector<float> GenerateGeometryVertices();
    static std::vector<int> GenerateTopologyIndices();
    static std::vector<float> vertices;
    static std::vector<int> indices;

    QVector4D m_color = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    QPropertyNotifier selectedNotifier;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    std::shared_ptr<CompositeAware> p_CompositeAware;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit Cube(const QString& name, QVector3D position);
};


#endif //SMOCAD_CUBE_H
