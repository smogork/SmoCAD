//
// Created by ksm on 5/14/22.
//

#ifndef SMOCAD_PLAINC0_H
#define SMOCAD_PLAINC0_H

#include <QPropertyNotifier>
#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Mesh.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Components/UVParams.h"


#define PATCH_SIZE 4

class PlainC0: public IEntity
{
Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);

protected:
    class Mesh m_mesh;
    QPropertyNotifier meshDrawingNotifier;
    QPropertyNotifier meshColorNotifier;
    QPropertyNotifier selectedNotifier;

    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    int GetIndexCount();

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<DynamicDrawing> p_Drawing;
    std::shared_ptr<TransformCollection> p_Collection;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<UVParams> p_UV;
    QProperty<QColor> MeshColor;
    QProperty<QColor> PlainColor;

    PlainC0(const QString& name, int width, int height);
};

#endif //SMOCAD_PLAINC0_H
