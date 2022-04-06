//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_TORUS_H
#define SMOCAD_TORUS_H


#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/UVParams.h"

class Torus: public IEntity
{
private:
    int GetIndexCount();
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader);

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    
private slots:
    void UVChanged(std::shared_ptr<UVParamsChanged> e);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<UVParams> p_UV;//R - U, r - V

    explicit Torus(QVector3D position);
};


#endif //SMOCAD_TORUS_H
