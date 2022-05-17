//
// Created by ksm on 5/17/22.
//

#ifndef SMOCAD_UVPLAINCREATOR_H
#define SMOCAD_UVPLAINCREATOR_H

#include "IComponent.h"
#include "Transform.h"
#include "Scene/Entities/Plains/PlainC0.h"

class UVPlaneCreator: public IComponent
{
    Q_OBJECT
public:
    QProperty<float> U;
    QProperty<float> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;

    explicit UVPlaneCreator(unsigned int oid);
    ~UVPlaneCreator() override;

    static std::shared_ptr<UVPlaneCreator> CreateRegisteredComponent(unsigned int oid, int U, int V);
    void UnregisterComponent();

    std::shared_ptr<PlainC0> CreatePlainC0(std::shared_ptr<Transform> startPoint);
};

#endif //SMOCAD_UVPLAINCREATOR_H
