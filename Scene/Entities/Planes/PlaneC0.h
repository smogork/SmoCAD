//
// Created by ksm on 5/14/22.
//

#ifndef SMOCAD_PLANEC0_H
#define SMOCAD_PLANEC0_H

#include <QPropertyNotifier>
#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Entities/Mesh.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Components/UVParams.h"
#include "BasePlane.h"

#define PATCH_SIZE 4

/*class PlaneC0: public IEntity
{
Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();

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

    PlaneC0(const QString& name, bool isPipe, int width, int height);
    ~PlaneC0();
};*/

class PlaneC0: public BasePlane
{
Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();

protected:
    QPropertyNotifier selectedNotifier;

    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyIndices() override;
    int GetIndexCount() override;

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    PlaneC0(const QString& name, bool isPipe, int countU, int countV);
};

#endif //SMOCAD_PLANEC0_H
