//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_TRANSFORM_H
#define SMOCAD_TRANSFORM_H


#include <QVector3D>
#include "IComponent.h"

class Transform: public IComponent
{
protected:

public:
    QVector3D Position;
    QVector3D Rotation;
    QVector3D Scale;

    static std::shared_ptr<Transform> CreateRegisteredComponent(unsigned int oid, QVector3D position);
    void UnregisterComponent();

    explicit Transform(unsigned int oid);
    Transform(unsigned int oid, QVector3D position, QVector3D rotation, QVector3D scale);
    ~Transform() override;

    virtual QMatrix4x4 GetModelMatrix();
    void DecomposeTransformations(QMatrix4x4 transform);
};


#endif //SMOCAD_TRANSFORM_H
