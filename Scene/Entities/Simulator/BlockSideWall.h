//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_BLOCKSIDEWALL_H
#define SMOCAD_BLOCKSIDEWALL_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class BlockSideWall: public IEntity
{
private:
    QMatrix4x4 m_simulatorMtx;
    double m_widthX, m_widthY, m_height;
    int m_vertexWidthX, m_vertexWidthY;
    int m_indexCount;
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    void CreateVerticesForWall(const std::vector<QVector3D> &wall, int vertexWidth, std::vector<float> &res, int scaleU, int scaleV);
    float ScaleTexCoord(int vertexNum, int vertexCount, int scaleFlag);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    
    BlockSideWall(QVector3D pos, double widthX, double widthY, double height, int vertexWidthX, int vertexWidthY);
};

#endif //SMOCAD_BLOCKSIDEWALL_H
