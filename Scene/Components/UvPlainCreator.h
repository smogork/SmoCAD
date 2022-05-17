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

private:
    std::shared_ptr<Transform> m_transform = nullptr;
public:
    QProperty<float> U;
    QProperty<float> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;

    explicit UVPlaneCreator(unsigned int oid);
    ~UVPlaneCreator() override;

    static std::shared_ptr<UVPlaneCreator> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, int U, int V);
    void UnregisterComponent();

    std::shared_ptr<PlainC0> CreatePlainC0();
};

#endif //SMOCAD_UVPLAINCREATOR_H
