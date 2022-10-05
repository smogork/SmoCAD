//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_UTILITES_H
#define SMOCAD_UTILITES_H

#include <QVector4D>
#include <QColor>

#include "Utils/Float3.h"

#define ASSIGN_NOTIFIER_FUNCTION(F)  std::bind(F, this)
#define ASSIGN_CONTEXT_SCENE_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)
#define ASSIGN_CONTEXT_LIST_FUNCTION(F)  std::bind(F, this, std::placeholders::_1, std::placeholders::_2)
#define ASSIGN_CONTEXT_LIST_FUNCTION_IGNORE_SELECTION(F)  std::bind(F, this, std::placeholders::_1)

QVector4D ColorToVector4D(QColor c);
float wrap(float v, float min, float max);
int wrap(int v, int min, int max);

MG1::Float3 SerializeQVector3D(QVector3D v);
QVector3D DeserializeFloat3(MG1::Float3 v);


#endif //SMOCAD_UTILITES_H
