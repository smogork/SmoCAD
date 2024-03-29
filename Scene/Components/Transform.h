//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_TRANSFORM_H
#define SMOCAD_TRANSFORM_H


#include <QVector3D>
#include <QProperty>
#include "IComponent.h"

class Transform: public IComponent
{
    Q_OBJECT
public:
    bool Locked = false;
    QProperty<QVector3D> Position;
    QProperty<QVector3D> Rotation;
    QProperty<QVector3D> Scale;

    static QVector3D GetXAxis() { return {1.0, 0.0, 0.0}; }
    static QVector3D GetYAxis() { return {0.0, 1.0, 0.0}; }
    static QVector3D GetZAxis() { return {0.0, 0.0, 1.0}; }

    static std::shared_ptr<Transform> CreateRegisteredComponent(unsigned int oid, QVector3D position);
    void UnregisterComponent();

    explicit Transform(unsigned int oid);
    Transform(unsigned int oid, QVector3D position, QVector3D rotation, QVector3D scale);
    ~Transform() override;

    virtual QMatrix4x4 GetModelMatrix();
    void DecomposeTransformations(QMatrix4x4 transform);

    void SetPosition(QVector3D pos);
};


#endif //SMOCAD_TRANSFORM_H
