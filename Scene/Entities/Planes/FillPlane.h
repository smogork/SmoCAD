//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_FILLPLANE_H
#define SMOCAD_FILLPLANE_H

#include "BasePlane.h"
#include "Scene/Components/FillAware.h"
#include "GregoryMesh.h"
#include "Scene/Systems/FillAwareSystem.h"

class FillPlane: public BasePlane
{
Q_OBJECT
private slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);
    void PointRemovedFromCollection();

protected:
    QPropertyNotifier selectedNotifier;
    QPropertyNotifier gmeshShowNotifier;
    GregoryMesh m_gmesh;
    
    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyIndices() override;
    int GetIndexCount() override;
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;
    
    FillPlane(const QString& name, const FillLoop& planeLoop);
};

#endif //SMOCAD_FILLPLANE_H
