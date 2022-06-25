//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONAWARE_H
#define SMOCAD_INTERSECTIONAWARE_H

#include "Scene/Components/IComponent.h"
#include "Scene/Components/UVParams.h"

#define ASSIGN_UV_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)

class IntersectionAware: public IComponent
{
public:
    std::shared_ptr<UVParams> p_UV;
    QProperty<bool> UWraps, VWraps;
    QProperty<float> UMin, UMax;
    QProperty<float> VMin, VMax;
    std::function<QVector3D(QVector2D uv)> SceneFunction;
    std::function<QVector3D(QVector2D uv)> SceneFunctionDerU;
    std::function<QVector3D(QVector2D uv)> SceneFunctionDerV;
    
    static std::shared_ptr<IntersectionAware> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv);
    void UnregisterComponent();
    
    explicit IntersectionAware(unsigned int oid);
    ~IntersectionAware() override;

    QVector4D FindClosestPoints(std::shared_ptr<IntersectionAware> other, int density = 10);
    QVector2D FindClosestPoints(QVector3D pos, int density = 10);
    bool ArgumentsInsideDomain(QVector2D args);
};

#endif //SMOCAD_INTERSECTIONAWARE_H
