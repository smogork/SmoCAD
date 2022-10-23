//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_BLOCKLOWERWALL_H
#define SMOCAD_BLOCKLOWERWALL_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

//DOlna sciana zawsze jest w Y = 0 wzgledem ukladu symulatora
class BlockLowerWall: public IEntity
{
private:
    std::weak_ptr<Transform> m_simulatorTransform;
    double m_widthX, m_widthY;
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    
public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    
    BlockLowerWall(QVector3D pos, std::shared_ptr<Transform> simulatorTransform, double widthX, double widthY);
};

#endif //SMOCAD_BLOCKLOWERWALL_H
