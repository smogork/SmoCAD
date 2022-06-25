//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_PLANEC2_H
#define SMOCAD_PLANEC2_H

#include "BasePlane.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Components/Awares/IntersectionAware.h"

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
    int GetVertexCount(bool isPipe);
    void GetIndexesOfPatch(int uPatch, int vPatch, std::vector<int>& indices);

    void SerializingFunction(MG1::Scene& scene);
    void InitializeUV(bool isPipe);
    static std::vector<QVector3D> FromBSplineToBernstein(const std::vector<QVector3D>& bspline);
    
    QVector3D PlaneC2Func(QVector2D uv);
    QVector3D PlaneC2FuncDerU(QVector2D uv);
    QVector3D PlaneC2FuncDerV(QVector2D uv);

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<IntersectionAware> p_Intersection;

    PlaneC2(const QString& name, bool isPipe, int countU, int countV);
    explicit PlaneC2(const MG1::BezierSurfaceC2 &p2);
    void InitObject(const QString &name, bool isPipe, int countU, int countV);

    static std::vector<std::shared_ptr<Point>>
    CreatePointsForPlane(QVector3D startPos, const QString &name, bool isPipe, int U, int V, float width, float height);
};

#endif //SMOCAD_PLANEC2_H
