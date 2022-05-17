//
// Created by ksm on 5/17/22.
//

#include "UvPlainCreator.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/UvPlaneCreatorSystem.h"

UVPlaneCreator::UVPlaneCreator(unsigned int oid) : IComponent(oid, UV_PLANE_CREATOR)
{
    U = 5;
    V = 1;
    UDensity = 32;
    VDensity = 16;
}

UVPlaneCreator::~UVPlaneCreator()
{
    UnregisterComponent();
}

std::shared_ptr<UVPlaneCreator>
UVPlaneCreator::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, int U, int V)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<UVPlaneCreatorSystem>().lock();
        auto res = system->CreateRegistered(oid);
        res->U = U;
        res->V = V;
        res->m_transform = transform;
        return res;
    }
    return nullptr;
}

void UVPlaneCreator::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<UVPlaneCreatorSystem>().lock())
            system->Unregister(GetAttachedObjectID());
    }
}

std::shared_ptr<PlainC0> UVPlaneCreator::CreatePlainC0(const QString &name)
{
    //Uwtorz punkty budujace plaszczyzne i dodaj je do sceny
    auto points = CreatePoints(name, U, V);

    //Dodaj punkty w odpowiedniej kolejnosci do plaszczyzny
    auto plane = std::make_shared<PlainC0>(name, (PATCH_SIZE - 1) * U + 1, (PATCH_SIZE - 1) * V + 1);

    for (int i = 0; i < U; ++i)
        for (int j = 0; j < V; ++j)
        {
            auto patch_points = GetPointsForPatch(points, i, j);
            for (const auto &p: patch_points)
                plane->p_Collection->AddPoint(p);
        }

    //przenies parametry UV do plaszczyzny
    plane->p_UV->UDensity = *UDensity;
    plane->p_UV->VDensity = *VDensity;

    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(plane);
    return plane;
}

std::vector<std::shared_ptr<Point>> UVPlaneCreator::CreatePoints(const QString &name, int w, int h, Plane p)
{
    std::vector<std::shared_ptr<Point>> res;

    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < (PATCH_SIZE - 1) * h + 1; ++i)
            for (int j = 0; j < (PATCH_SIZE - 1) * w + 1; ++j)
            {
                auto p = std::make_shared<Point>(QString("P_%0_%1%2").arg(name).arg(i).arg(j),
                                                 QVector3D((float) i / (PATCH_SIZE - 1), 0,
                                                           (float) j / (PATCH_SIZE - 1)) + m_transform->Position);
                res.emplace_back(p);
                scene->AddObjectExplicitPosition(p);
            }
    }

    return res;
}

std::vector<std::shared_ptr<CollectionAware>>
UVPlaneCreator::GetPointsForPatch(std::vector<std::shared_ptr<Point>> &points, int pi, int pj)
{
    std::vector<std::shared_ptr<CollectionAware>> res(16);

    int second_dim = (PATCH_SIZE - 1) * U + 1;
    for (int i = 0; i < PATCH_SIZE; ++i)
        for (int j = 0; j < PATCH_SIZE; ++j)
        {
            int wIdx = pi * (PATCH_SIZE - 1) + i;
            int hIdx = pj * (PATCH_SIZE - 1) + j;
            res[i * PATCH_SIZE + j] = points[
                    hIdx * second_dim + wIdx]->p_CollectionAware;
        }

    return res;
}
