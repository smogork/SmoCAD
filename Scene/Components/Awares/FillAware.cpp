//
// Created by ksm on 6/13/22.
//


#include "FillAware.h"
#include "Scene/Systems/ISystem.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Awares/FillAwareSystem.h"

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

std::vector<std::shared_ptr<CollectionAware>> FillAware::GetPointsFromEdge(FillEdge edge)
{
    auto points = p_Collection->GetVectorAwares();
    if (edge == FillEdge::ZERO)
        return {points[0].lock(), points[1].lock(), points[2].lock(), points[3].lock(),
                points[4].lock(), points[5].lock(), points[6].lock(), points[7].lock(),};
    
    if (edge == FillEdge::ONE)
        return {points[3].lock(), points[7].lock(), points[11].lock(),
                points[15].lock(), points[2].lock(), points[6].lock(),
                points[10].lock(), points[14].lock()};
    
    if (edge == FillEdge::TWO)
        return {points[12].lock(), points[13].lock(), points[14].lock(),
                points[15].lock(), points[8].lock(), points[9].lock(),
                points[10].lock(), points[11].lock()};
    
    if (edge == FillEdge::THREE)
        return {points[0].lock(), points[4].lock(), points[8].lock(),
                points[12].lock(), points[1].lock(), points[5].lock(),
                points[9].lock(), points[13].lock()};
    
    throw std::runtime_error("Cannot get points for edge none");
}

std::vector<std::pair<CrossingPoint, CrossingPoint>> FillAware::GetCrossingPointsWith(const std::shared_ptr<FillAware> &other)
{
    //JEzeli nie ma dokladnie 16 punktow (jeden platek) to w ogole nie rozwazamy przeciec
    if (p_Collection->Size() != 16 or other->p_Collection->Size() != 16)
        return {};
    
    auto one_points = p_Collection->GetVectorAwares();
    auto two_points = other->p_Collection->GetVectorAwares();
    
    std::vector<std::shared_ptr<CollectionAware>> one_corners = {one_points[0].lock(), one_points[3].lock(),
                                                                 one_points[15].lock(), one_points[12].lock()};
    std::vector<std::shared_ptr<CollectionAware>> two_corners = {two_points[0].lock(), two_points[3].lock(),
                                                                 two_points[15].lock(), two_points[12].lock()};
    std::vector<std::pair<CrossingPoint, CrossingPoint>> res;
    for (int one_i = 0; one_i < 4; ++one_i)
    {
        for (int two_i = 0; two_i < 4; ++two_i)
        {
            if (one_corners[one_i]->GetAttachedObjectID() == two_corners[two_i]->GetAttachedObjectID())
                res.push_back(std::make_pair((CrossingPoint)one_i, (CrossingPoint)two_i));
        }
    }
    return res;
}

FillEdge FillAware::MapPointsToEdges(CrossingPoint u, CrossingPoint v)
{
    if ((u == CrossingPoint::ONE && v == CrossingPoint::ZERO) or
        (u == CrossingPoint::ZERO && v == CrossingPoint::ONE))
        return FillEdge::ZERO;
    
    if ((u == CrossingPoint::ONE && v == CrossingPoint::TWO) or
        (u == CrossingPoint::TWO && v == CrossingPoint::ONE))
        return FillEdge::ONE;
    
    if ((u == CrossingPoint::THREE && v == CrossingPoint::TWO) or
        (u == CrossingPoint::TWO && v == CrossingPoint::THREE))
        return FillEdge::TWO;
    
    if ((u == CrossingPoint::THREE && v == CrossingPoint::ZERO) or
        (u == CrossingPoint::ZERO && v == CrossingPoint::THREE))
        return FillEdge::THREE;
    
    return FillEdge::NONE;
}
