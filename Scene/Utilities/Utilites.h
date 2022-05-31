//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_UTILITES_H
#define SMOCAD_UTILITES_H

#include <QVector4D>
#include <QColor>

#include "Utils/Float3.h"

#define ASSIGN_NOTIFIER_FUNCTION(F)  std::bind(F, this)
#define ASSIGN_CONTEXT_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)

QVector4D ColorToVector4D(QColor c);
MG1::Float3 SerializeQVector3D(QVector3D v);
QVector3D DeserializeFloat3(MG1::Float3 v);

#endif //SMOCAD_UTILITES_H
