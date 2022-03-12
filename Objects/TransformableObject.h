//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_TRANSFORMABLEOBJECT_H
#define SMOCAD_TRANSFORMABLEOBJECT_H


#include <QVector3D>
#include <QMatrix4x4>

class TransformableObject
{
public:
    QVector3D Position;
    QVector3D Rotation;
    QVector3D Scale;

    explicit TransformableObject(QVector3D position);

    TransformableObject(QVector3D position, QVector3D rotation, QVector3D scale);

    QMatrix4x4 GetModelMatrix();
};


#endif //SMOCAD_TRANSFORMABLEOBJECT_H
