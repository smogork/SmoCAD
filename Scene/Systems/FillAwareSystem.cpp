//
// Created by ksm on 6/13/22.
//

#include "FillAwareSystem.h"
#include "Mathematics/PointShapes.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/SceneECS.h"
#include "Scene/Entities/Planes/FillPlane.h"
#include "Controls/EntityContextMenu.h"

#include <QString>
#include<queue>

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
    
    auto loops = FindFillLoops(awares);
    
    if (auto scene = SceneECS::Instance().lock())
    {
        for (const auto& loop : loops)
        {
            std::shared_ptr<FillPlane> p = std::make_shared<FillPlane>("NewFillPlane", loop);
            scene->AddObject(p);
            p->p_Selected->Selected = true;
        }
        
        EntityContextMenu::MakeControlsUpdate(0);
        EntityContextMenu::MakeRepaint();
    }
}

bool FillAwareSystem::IsHoleToFill(const std::vector<std::shared_ptr<FillAware>> &awares)
{
    int test = FindFillLoops(awares).size();
    qDebug() << "Found " << test << " holes";
    return test > 0;
}

std::vector<FillLoop> FillAwareSystem::FindFillLoops(const std::vector<std::shared_ptr<FillAware>> &awares)
{
    //Czy to jest potrzebne?
    std::vector<std::shared_ptr<FillAware>> patches;
    for (const auto &p: awares)
        if (p->p_Collection->Size() == 16)
            patches.push_back(p);
    
    if (patches.size() != 3)//tak na wszelki wypadek
        return {};
    
    //zbuduj graf krawedzi
    NGraph::Graph patchesEdges;
    patchesEdges.set_undirected();
    for (int i = 0; i < patches.size(); ++i)
    {
        //tworzenie krawedzi laczocych wierzcholki zgodnie z numeracja w CrossingPoint
        patchesEdges.insert_edge(4 * i, 4 * i + 1);
        patchesEdges.insert_edge(4 * i + 1, 4 * i + 2);
        patchesEdges.insert_edge(4 * i + 2, 4 * i + 3);
        patchesEdges.insert_edge(4 * i + 3, 4 * i);
        
        //dodanie krawedzi miedzy platkami
        for (int j = i + 1; j < patches.size(); ++j)
        {
            auto crossings = patches[i]->GetCrossingPointsWith(patches[j]);
            for (auto cross: crossings)
                patchesEdges.insert_edge(4 * i + (int) cross.first, 4 * j + (int) cross.second);
        }
    }
    
    for (auto e : patchesEdges.edge_list())
        qDebug()  << e;
    
    std::vector<FillLoop> res;
    for (int i = 0; i < 4 * patches.size(); ++i)
    {
        FillLoop found = StartSearchFrom(patchesEdges, i, patches);
        if (!found.empty())
            res.push_back(found);
    }
    
    return res;
}

FillLoop
FillAwareSystem::StartSearchFrom(NGraph::Graph &g, int v, const std::vector<std::shared_ptr<FillAware>> &patches)
{
    //znajdz 6-krawedziowy cykl w tym grafie dla kazdego startujac z kazdego wierzcholka
    std::stack<std::pair<int, int>> search;//(id wierzcholka, krok przeszukiwania)
    std::vector<int> res(7);
    std::vector<bool> visited(g.num_vertices());
    bool found = false;
    for (int i = 0; i < visited.size(); ++i)
        visited[i] = false;
    
    //przegladanie wglab
    search.push({v, 0});
    while (!search.empty())
    {
        std::pair<int, int> current = search.top();
        search.pop();
        
        if (current.second > 6)//przekroczylismy odzwolona dlugosc
            continue;
        
        if (current.second == 6 && current.first == v)//znaleziono cykl
        {
            found = true;
            break;
        }
        
        if (visited[current.first])//juz odwiedzony - nie idziemy dalej
            continue;
        visited[current.first] = true;
        
        res[current.second] = current.first;//numer platka
        
        std::set<uint> neighs_out = g.out_neighbors(current.first);
        std::set<uint> neighs_in = g.in_neighbors(current.first);
        if (current.second % 2 == 0)//wyszykujemy krawedzi do innych platkow
        {
            for (uint u: neighs_out)
            {
                if (u / 4 != current.first / 4)
                    search.push(std::make_pair(u, current.second + 1));
            }
            for (uint u: neighs_in)
            {
                if (u / 4 != current.first / 4)
                    search.push(std::make_pair(u, current.second + 1));
            }
        } else//wyszukujemy krawedzie do tego samego platka
        {
            for (uint u: neighs_out)
            {
                if (u / 4 == current.first / 4)
                    search.push(std::make_pair(u, current.second + 1));
            }
            for (uint u: neighs_in)
            {
                if (u / 4 == current.first / 4)
                    search.push(std::make_pair(u, current.second + 1));
            }
        }
    }
    
    if (!found)
        return {};
    
    
    //Przetworz res na FillLoop
    FillLoop loop;
    for (int i = 0; i < 3; ++i)
    {
        //dodac cykl do wyniku
        int patchv1 = res[(2 * i + 2) % 6] % 4;//numer wierzcholka w patch
        int patchv2 = res[(2 * i + 1) % 6] % 4;
        
        loop.loop.push_back(std::make_pair(patches[res[(2 * i + 1) % 6] / 4],
                                           FillAware::MapPointsToEdges((CrossingPoint) patchv1,
                                                                       (CrossingPoint) patchv2)));
    
        //usuwamy krawedzie mostowe z grafu
        g.remove_edge(res[(2 * i) % 6], res[(2 * i + 1) % 6]);
    }
    return loop;
}

std::vector<std::shared_ptr<CollectionAware>> FillLoop::GetNormalizedLoopPoints() const
{
    if (loop.size() != 3)
        throw new std::runtime_error("Cannot normalize loop of non 3 elements");
    
    auto one_points = loop[0].first->GetPointsFromEdge(loop[0].second);
    auto two_points = loop[1].first->GetPointsFromEdge(loop[1].second);
    auto three_points = loop[2].first->GetPointsFromEdge(loop[2].second);
    
    //ustaw punkty aby tworzyly jeden ciag
    if (one_points[3]->GetAttachedObjectID() == three_points[3]->GetAttachedObjectID() or
        one_points[3]->GetAttachedObjectID() == three_points[0]->GetAttachedObjectID())
        for (int i = 0; i < 2; ++i)
        {
            std::swap(one_points[i], one_points[3 - i]);
            std::swap(one_points[i + 4], one_points[3 - i + 4]);
        }
    
    if (one_points[3]->GetAttachedObjectID() == two_points[3]->GetAttachedObjectID())//dwojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(two_points[i], two_points[3 - i]);
            std::swap(two_points[i + 4], two_points[3 - i + 4]);
        }
    
    if (two_points[3]->GetAttachedObjectID() == three_points[3]->GetAttachedObjectID())//trojka jest w zla strone
        for (int i = 0; i < 2; ++i)
        {
            std::swap(three_points[i], three_points[3 - i]);
            std::swap(three_points[i + 4], three_points[3 - i + 4]);
        }
    
    std::vector<std::shared_ptr<CollectionAware>> res;
    for (int j = 0; j < one_points.size(); ++j) res.push_back(one_points[j]);
    for (int j = 0; j < two_points.size(); ++j) res.push_back(two_points[j]);
    for (int j = 0; j < three_points.size(); ++j) res.push_back(three_points[j]);
    
    return res;
}
