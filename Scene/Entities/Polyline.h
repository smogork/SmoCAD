//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_POLYLINE_H
#define SMOCAD_POLYLINE_H


#include "IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"

//Entity reprezentujący łamaną w przestrzeni
class Polyline: public IEntity
{
private:
    static std::vector<float> GenerateGeometryVertices();
    static std::vector<int> GenerateTopologyIndices();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;

    Polyline();
};


#endif //SMOCAD_POLYLINE_H
