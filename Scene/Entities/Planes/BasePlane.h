//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_BASEPLANE_H
#define SMOCAD_BASEPLANE_H

//Width - U
//Height - V

#include <QPropertyNotifier>
#include "Scene/Entities/IEntity.h"
#include "Scene/Entities/Mesh.h"
#include "Scene/Components/UVParams.h"

class BasePlane: public IEntity
{
    Q_OBJECT

protected:
    Mesh m_mesh;
    QPropertyNotifier meshDrawingNotifier;
    QPropertyNotifier meshColorNotifier;

    virtual std::vector<float> GenerateGeometryVertices() = 0;
    virtual std::vector<int> GenerateTopologyIndices() = 0;
    virtual int GetIndexCount() = 0;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    std::shared_ptr<UVParams> p_UV;
    QProperty<QColor> MeshColor;
    QProperty<QColor> PlaneColor;

    BasePlane(uint cid, bool isPipe, int countU, int countV);
    ~BasePlane() override;
};

#endif //SMOCAD_BASEPLANE_H
