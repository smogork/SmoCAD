//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_PLANEC2_H
#define SMOCAD_PLANEC2_H

#include "BasePlane.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"
#include "Scene/Entities/Points/Point.h"

class PlaneC2: public BasePlane
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

    PlaneC2(const QString& name, bool isPipe, int countU, int countV);

    static std::vector<std::shared_ptr<Point>>
    CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width, float height);
};

#endif //SMOCAD_PLANEC2_H
