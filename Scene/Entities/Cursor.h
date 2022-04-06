//
// Created by ksm on 4/6/22.
//

#ifndef SMOCAD_CURSOR_H
#define SMOCAD_CURSOR_H


#include "IEntity.h"
#include "Renderer/ShaderWrapper.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class Cursor: public IEntity
{
private:
    static std::vector<float> GenerateGeometryVertices();
    static std::vector<int> GenerateTopologyIndices();
    static std::vector<float> vertices;
    static std::vector<int> indices;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;

    explicit Cursor(QVector3D position);
};


#endif //SMOCAD_CURSOR_H
