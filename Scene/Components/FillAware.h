//
// Created by ksm on 6/13/22.
//

#ifndef SMOCAD_FILLAWARE_H
#define SMOCAD_FILLAWARE_H

#include "IComponent.h"
#include "TransformCollection.h"

enum class FillEdge
{
    NONE,
    ZERO,//0,1,2,3
    ONE,//3,7,11,15
    TWO,//12,13,14,15
    THREE,//0,4,8,12
};

enum class CrossingPoint
{
    NONE,
    ZERO,//0
    ONE,//3
    TWO,//15
    THREE//12
};


class FillAware: public IComponent
{
public:
    std::shared_ptr<TransformCollection> p_Collection;
    
    static std::shared_ptr<FillAware> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<TransformCollection>);
    void UnregisterComponent();
    
    explicit FillAware(unsigned int oid);
    ~FillAware() override;
    
    CrossingPoint GetCrossingPointWith(const std::shared_ptr<FillAware>& other);
    FillEdge GetFillEdgeWith(const std::shared_ptr<FillAware>& one, const std::shared_ptr<FillAware>& two);
    std::vector<QVector3D> GetPointsFromEdge(FillEdge edge);
};

#endif //SMOCAD_FILLAWARE_H
