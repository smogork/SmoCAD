//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_CUBE_H
#define SMOCAD_CUBE_H


#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class Cube: public IEntity
{
private:
    static std::vector<float> GenerateGeometryVertices();
    static std::vector<int> GenerateTopologyIndices();
    static std::vector<float> vertices;
    static std::vector<int> indices;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;

    explicit Cube(QVector3D position);
};


#endif //SMOCAD_CUBE_H
