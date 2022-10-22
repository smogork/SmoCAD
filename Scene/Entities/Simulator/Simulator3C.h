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
#include "CutterPath.h"
#include "BlockUpperWall.h"
#include "BlockLowerWall.h"
#include "BlockSideWall.h"
#include "BlockParameters.h"

/**
 * To entity lamie wszystkei poprzednie zasady o dzialaniu na componentach zamiast na Entity - trudno, mam wyjebane.
 * Jest to zbyt zlozony byt dla mojej technologii. Bedzie on zawierac w sobie wiele entity i zarzadzac nimi.
 */
class Simulator3C: public IEntity
{
private:
    //CutterObject m_cutter;
    //CutterPath m_cutterPath;
    /*BlockUpperWall m_blockUpper;
    BlockLowerWall m_blockLower;
    BlockSideWall m_blockSide[4];
    BlockParameters m_blockParams;*/
    
public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    std::shared_ptr<SceneElement> p_SceneElement;
    std::shared_ptr<Selectable> p_Selected;
    
    explicit Simulator3C(const QString& name);
    Simulator3C(const QString& name, QVector3D position);
};

#endif //SMOCAD_SIMULATOR3C_H
