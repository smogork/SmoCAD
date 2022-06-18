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
#include "Scene/Components/FillAware.h"

class PlaneC0 : public BasePlane
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
    int GetVertexCount(bool isPipe);

    void SerializingFunction(MG1::Scene& scene);

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<FillAware> p_FillAware;

    PlaneC0(const QString &name, bool isPipe, int countU, int countV);
    explicit PlaneC0(const MG1::BezierSurfaceC0 &p0);
    void InitObject(const QString &name, bool isPipe, int countU, int countV);

    static std::vector<std::shared_ptr<Point>>
    CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width, float height);
};

#endif //SMOCAD_PLANEC0_H
