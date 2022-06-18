//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_GREGORYMESH_H
#define SMOCAD_GREGORYMESH_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"

class GregoryMesh: public IEntity
{
private:
    int m_indexCount = 0;
    std::vector<int> GenerateTopologyIndices(int patchCount);
    int GetIndexCount();
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    
public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    QColor DrawingColor = DefaultColor;
    
    GregoryMesh();
    
    void SetGregoryPoints(const std::vector<QVector3D>& points);
};

#endif //SMOCAD_GREGORYMESH_H
