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
    Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);

private:
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QColor DrawingColor = DefaultColor;

    Polyline();
};


#endif //SMOCAD_POLYLINE_H
