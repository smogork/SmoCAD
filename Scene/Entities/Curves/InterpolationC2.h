//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_INTERPOLATIONC2_H
#define SMOCAD_INTERPOLATIONC2_H

#include "BaseCurve.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/SceneElement.h"

class InterpolationC2: public BaseCurve
{
    Q_OBJECT
protected:
    std::list<std::unique_ptr<InvisiblePoint>> m_bernsteinPoints;
    QPropertyNotifier selectedNotifier;

    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyIndices() override;
    int GetIndexCount() override;
    void SerializingFunction(MG1::Scene& scene);

protected slots:
    void OnCollectionModified();
    void OnSinglePointModified(QVector3D pos, unsigned int changedOID);

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit InterpolationC2(const QString& name);
    explicit InterpolationC2(const MG1::InterpolatedC2& i2);
    void InitObject(const QString& name);
};

#endif //SMOCAD_INTERPOLATIONC2_H
