//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_TORUS_H
#define SMOCAD_TORUS_H

#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/UVParams.h"
#include "Scene/Components/CompositeAware.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"

class Torus: public IEntity
{
private:
    QColor m_color = DefaultColor;
    QPropertyNotifier selectedNotifier;
    QPropertyNotifier compositeNotifier;

    void HandleColors();
    int GetIndexCount();
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    void SerializingFunction(MG1::Scene& scene);

    void UVChanged();
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    QPropertyNotifier uNotifier;
    QPropertyNotifier vNotifier;
    QPropertyNotifier udNotifier;
    QPropertyNotifier vdNotifier;

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<UVParams> p_UV;//R - U, r - V
    std::shared_ptr<CompositeAware> p_CompositeAware;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit Torus(const QString& name);
    explicit Torus(const MG1::Torus& serializedObj);
    Torus(const QString& name, QVector3D position);
    void InitObject(const QString& name, QVector3D position);
};


#endif //SMOCAD_TORUS_H
