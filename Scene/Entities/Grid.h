//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_GRID_H
#define SMOCAD_GRID_H

#include "IEntity.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class Grid: public IEntity
{
private:
    constexpr static const QVector4D gridColor = QVector4D(0.5f, 0.5f, 0.5f, 1.0f);
    static const int gridVerticesCount = 100;
    constexpr static const float gridOffset = 1.0f;

    static std::vector<float> GenerateGridVertices();
    static std::vector<int> GenerateGridIndices();

    static std::vector<float> gridVertices;
    static std::vector<int> gridIndices;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<StaticDrawing> p_Drawing;

    Grid();
};


#endif //SMOCAD_GRID_H
