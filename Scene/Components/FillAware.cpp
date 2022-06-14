//
// Created by ksm on 6/13/22.
//


#include "FillAware.h"
#include "Scene/Systems/ISystem.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/FillAwareSystem.h"

std::shared_ptr<FillAware>
FillAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<TransformCollection> col)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<FillAwareSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->p_Collection = col;
            return item;
        }
    }
    return nullptr;
}

void FillAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<FillAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

FillAware::FillAware(unsigned int oid) : IComponent(oid, FILL_AWARE)
{

}

FillAware::~FillAware()
{
    UnregisterComponent();
}

CrossingPoint FillAware::GetCrossingPointWith(const std::shared_ptr<FillAware> &other)
{
    //JEzeli nie ma dokladnie 16 punktow (jeden platek) to w ogole nie rozwazamy przeciec
    if (p_Collection->Size() != 16 or other->p_Collection->Size() != 16)
        return CrossingPoint::NONE;
    
    auto one_points = p_Collection->GetVectorPoints();
    auto two_points = other->p_Collection->GetVectorPoints();
    
    std::vector<std::shared_ptr<Transform>> one_corners = {one_points[0].lock(), one_points[3].lock(),
                                                           one_points[15].lock(), one_points[12].lock()};
    std::vector<std::shared_ptr<Transform>> two_corners = {two_points[0].lock(), two_points[3].lock(),
                                                           two_points[15].lock(), two_points[12].lock()};
    
    for (int one_i = 0; one_i < 4; ++one_i)
    {
        for (int two_i = 0; two_i < 4; ++two_i)
        {
            if (one_corners[one_i]->GetAttachedObjectID() == two_corners[two_i]->GetAttachedObjectID())
                return (CrossingPoint) (one_i + 1);
        }
    }
    return CrossingPoint::NONE;
}

FillEdge FillAware::GetFillEdgeWith(const std::shared_ptr<FillAware> &one, const std::shared_ptr<FillAware> &two)
{
    CrossingPoint onePoint = GetCrossingPointWith(one);
    CrossingPoint twoPoint = GetCrossingPointWith(two);
    
    if ((onePoint == CrossingPoint::ONE && twoPoint == CrossingPoint::ZERO) or
        (onePoint == CrossingPoint::ZERO && twoPoint == CrossingPoint::ONE))
        return FillEdge::ZERO;
    
    if ((onePoint == CrossingPoint::ONE && twoPoint == CrossingPoint::TWO) or
        (onePoint == CrossingPoint::TWO && twoPoint == CrossingPoint::ONE))
        return FillEdge::ONE;
    
    if ((onePoint == CrossingPoint::THREE && twoPoint == CrossingPoint::TWO) or
        (onePoint == CrossingPoint::TWO && twoPoint == CrossingPoint::THREE))
        return FillEdge::TWO;
    
    if ((onePoint == CrossingPoint::THREE && twoPoint == CrossingPoint::ZERO) or
        (onePoint == CrossingPoint::ZERO && twoPoint == CrossingPoint::THREE))
        return FillEdge::THREE;
    
    return FillEdge::NONE;
}

std::vector<QVector3D> FillAware::GetPointsFromEdge(FillEdge edge)
{
    auto points = p_Collection->GetVectorPoints();
    if (edge == FillEdge::ZERO)
        return {points[0].lock()->Position, points[1].lock()->Position, points[2].lock()->Position,
                points[3].lock()->Position, points[4].lock()->Position, points[5].lock()->Position,
                points[6].lock()->Position, points[7].lock()->Position,};
    
    if (edge == FillEdge::ONE)
        return {points[3].lock()->Position, points[7].lock()->Position, points[11].lock()->Position,
                points[15].lock()->Position, points[2].lock()->Position, points[6].lock()->Position,
                points[10].lock()->Position, points[14].lock()->Position};
    
    if (edge == FillEdge::TWO)
        return {points[12].lock()->Position, points[13].lock()->Position, points[14].lock()->Position,
                points[15].lock()->Position, points[8].lock()->Position, points[9].lock()->Position,
                points[10].lock()->Position, points[11].lock()->Position};
    
    if (edge == FillEdge::THREE)
        return {points[0].lock()->Position, points[4].lock()->Position, points[8].lock()->Position,
                points[12].lock()->Position, points[1].lock()->Position, points[5].lock()->Position,
                points[9].lock()->Position, points[13].lock()->Position};
    
    throw std::runtime_error("Cannot get points for edge none");
}
