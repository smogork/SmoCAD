//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_UVPARAMS_H
#define SMOCAD_UVPARAMS_H


#include "IComponent.h"
#include "Scene/Events/SystemEvents/UVParamsChanged.h"

class UVParams: public IComponent
{
    Q_OBJECT
private:
    float m_U;
    float m_V;
    int m_UDensity;
    int m_VDensity;

public:

    explicit UVParams(unsigned int);
    ~UVParams() override;

    static std::shared_ptr<UVParams> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    float U() { return m_U; }
    float V() { return m_V; }
    int UDensity() { return m_UDensity; }
    int VDensity() { return m_VDensity; }

    void SetU(float val);
    void SetV(float val);
    void SetUDensity(int val);
    void SetVDensity(int val);

    void EmitParamsChangedEvent();

signals:
    void ParametersChanged(std::shared_ptr<UVParamsChanged> event);
};


#endif //SMOCAD_UVPARAMS_H
