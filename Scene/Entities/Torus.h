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

class Torus: public IEntity
{
private:
    int GetIndexCount();
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

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

    explicit Torus(QVector3D position);
};


#endif //SMOCAD_TORUS_H
