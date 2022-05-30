//
// Created by ksm on 5/17/22.
//

#ifndef SMOCAD_PLANECREATOR_H
#define SMOCAD_PLANECREATOR_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Scene/Entities/Mesh.h"
#include "Scene/Components/UVParams.h"
#include "Scene/Entities/Points/VirtualPoint.h"
#include "Scene/Components/UvPlaneCreator.h"

class PlaneCreator: public IEntity
{
    Q_OBJECT
protected:
    class Mesh m_mesh;
    std::vector<std::shared_ptr<CollectionAware>> elements;
    std::vector<std::shared_ptr<VirtualPoint>> points;
    QPropertyNotifier uNotifier, vNotifier, wNotifier, hNotifier, pipeNotifier;
    QPropertyNotifier posNotifier, rotNotifier, scaleNotifier;


    void CreatePoints(int w, int h, Plane p = XY);
    void CreateTempMesh();

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<UVPlaneCreator> p_UVParams;

    explicit PlaneCreator(const QString& name, QVector3D pos = QVector3D());
};

#endif //SMOCAD_PLANECREATOR_H
