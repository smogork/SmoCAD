//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_UVPARAMS_H
#define SMOCAD_UVPARAMS_H

#include <QProperty>

#include "IComponent.h"
#include "Scene/Events/SystemEvents/UVParamsChanged.h"
#include "Scene/Entities/Plains/PlainC0.h"

class UVParams: public IComponent
{
    Q_OBJECT
public:
    QProperty<float> U;
    QProperty<float> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;

    explicit UVParams(unsigned int);
    ~UVParams() override;

    static std::shared_ptr<UVParams> CreateRegisteredComponent(unsigned int oid, int U, int V);
    void UnregisterComponent();

    //std::shared_ptr<PlainC0> CreatePlainC0(std::shared_ptr<Transform> startPoint);
};


#endif //SMOCAD_UVPARAMS_H
