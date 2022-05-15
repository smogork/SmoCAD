//
// Created by ksm on 5/15/22.
//

#ifndef SMOCAD_MESH_H
#define SMOCAD_MESH_H

#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "IEntity.h"

class Mesh: public IEntity
{
    Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);

private:
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    int GetIndexCount();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    QColor DrawingColor = DefaultColor;

    Mesh();
};

#endif //SMOCAD_MESH_H
