//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_UVPARAMS_H
#define SMOCAD_UVPARAMS_H

#define ASSIGN_UV_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)

#include <QProperty>

#include "IComponent.h"
#include "Scene/Events/SystemEvents/UVParamsChanged.h"



class UVParams: public IComponent
{
    Q_OBJECT

private:
    bool m_locked = false;

public:
    QProperty<float> U;
    QProperty<float> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;
    QProperty<bool> UWraps, VWraps;
    std::function<QVector3D(QVector2D uv)> SceneFunction;
    std::function<QVector3D(QVector2D uv)> SceneFunctionDerU;
    std::function<QVector3D(QVector2D uv)> SceneFunctionDerV;

    explicit UVParams(unsigned int);
    ~UVParams() override;

    static std::shared_ptr<UVParams> CreateRegisteredComponent(unsigned int oid, int U, int V);
    void UnregisterComponent();

    bool IsUVLocked();
    void LockEditUV(bool state = true);
};


#endif //SMOCAD_UVPARAMS_H
