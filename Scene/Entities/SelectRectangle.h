//
// Created by ksm on 6/11/22.
//

#ifndef SMOCAD_SELECTRECTANGLE_H
#define SMOCAD_SELECTRECTANGLE_H

#include "IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"

class SelectRectangle: public IEntity
{
protected:
    std::vector<float> GenerateGeometryVertices();
    static std::vector<int> GenerateTopologyIndices();
    std::vector<float> vertices;
    static std::vector<int> indices;
    QPropertyNotifier m_areaChangedNotifier;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    QProperty<QRect> SelectionArea;

    explicit SelectRectangle(QRect pos);
};

#endif //SMOCAD_SELECTRECTANGLE_H
