//
// Created by ksm on 6/13/22.
//

#include "FillAwareSystem.h"
#include "Mathematics/PointShapes.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/SceneECS.h"

#include <QString>

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
FillAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (IsHoleToFill(filteredObjects))
        res.push_back(
                std::make_pair(
                        "Fill hole",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateFillPlane(listContextOids);
                        }));
    
    return res;
}

void FillAwareSystem::CreateFillPlane(const std::vector<unsigned int> &listContextOids)
{
    auto awares = FilterObjects(listContextOids);
    
    auto one_points = awares[0]->GetPointsFromEdge(awares[0]->GetFillEdgeWith(awares[1], awares[2]));
    auto two_points = awares[1]->GetPointsFromEdge(awares[1]->GetFillEdgeWith(awares[0], awares[2]));
    auto three_points = awares[2]->GetPointsFromEdge(awares[2]->GetFillEdgeWith(awares[0], awares[1]));
    
    //ustaw punkty aby tworzyly jeden ciag
    if (one_points[3] == two_points[3])//dwojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(two_points[i], two_points[3 - i]);
            std::swap(two_points[i + 4], two_points[3 - i + 4]);
        }
        
    else if (two_points[3] == three_points[3])//trojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(three_points[i], three_points[3 - i]);
            std::swap(three_points[i + 4], three_points[3 - i + 4]);
        }
    
    
    //[TODO] Tutaj stworz obiekt ktory wciagnie tylko te 3 * 8 punktow i on juz sam zajmie sie ustawianiem pozostalyhc punktow wewnatrz
    
    //Wyzancz punkty budujace platki gregoriego
    std::vector<QVector3D> coords = PointShapes::CreateFillPlanePoints({one_points, two_points, three_points});
    
    //To juz we wnetrzu obiektu z gregorimi
    std::vector<std::shared_ptr<Point>> points;
    int counter = 0;
    if (auto scene = SceneECS::Instance().lock())
    {
        for (const QVector3D &coord: coords)
        {
            std::shared_ptr<Point> p = std::make_shared<Point>(QString("GreagoryPlanePoint%0").arg(counter++), coord);
            scene->AddObjectExplicitPosition(p);
            points.push_back(p);
        }
    }
}

bool FillAwareSystem::IsHoleToFill(const std::vector<std::shared_ptr<FillAware>> &awares)
{
    if (awares.size() != 3)
        return false;
    
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            if (i != j and awares[i]->GetCrossingPointWith(awares[j]) == CrossingPoint::NONE)
                return false;
                
        }
        
    return true;
}
