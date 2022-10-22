//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_CUTTERPATHPOLYGON_H
#define SMOCAD_CUTTERPATHPOLYGON_H

#include "Scene/Entities/IEntity.h"
#include "CutterPath.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class CutterPathPolygon: public IEntity
{
private:
    CutterPath m_pathData;

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
};

#endif //SMOCAD_CUTTERPATHPOLYGON_H
