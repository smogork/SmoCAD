//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_SIMULATOR3C_H
#define SMOCAD_SIMULATOR3C_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"
#include "Scene/Components/SceneElement.h"

#include "CutterObject.h"
#include "FileManagers/CutterPath.h"
#include "BlockUpperWall.h"
#include "BlockLowerWall.h"
#include "BlockSideWall.h"
#include "BlockParameters.h"
#include "Scene/Components/Simulator3CComponent.h"

class Simulator3C: public IEntity
{
private:


public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<Simulator3CComponent> p_Simulator;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;
    
    explicit Simulator3C(const QString& name);
    Simulator3C(const QString& name, QVector3D position);
};

#endif //SMOCAD_SIMULATOR3C_H
