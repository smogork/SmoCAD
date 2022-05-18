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
    Width = 3;
    Height = 2;
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
    int index_width = (PATCH_SIZE - 1) * U + 1;
    int index_height = (PATCH_SIZE - 1) * V + 1;

    //Uwtorz punkty budujace plaszczyzne i dodaj je do sceny
    auto points = CreatePoints(name, index_width, index_height);

    //Dodaj punkty w odpowiedniej kolejnosci do plaszczyzny
    auto plane = std::make_shared<PlainC0>(name, index_width, index_height);

    std::vector<std::shared_ptr<CollectionAware>> elems(points.size());
    for (int i = 0; i < elems.size(); ++i)
        elems[i] = points[i]->p_CollectionAware;
    plane->p_Collection->SetPoints(elems);
    plane->p_Collection->LockContent();

    //przenies parametry UV do plaszczyzny
    plane->p_UV->UDensity = *UDensity;
    plane->p_UV->VDensity = *VDensity;
    plane->p_UV->UWraps = *IsPipe;
    plane->p_UV->LockEditUV();

    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(plane);
    return plane;
}

std::vector<std::shared_ptr<Point>> UVPlaneCreator::CreatePoints(const QString &name, int w, int h, Plane p)
{
    std::vector<std::shared_ptr<Point>> res;

    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
            {
                std::shared_ptr<Point> p;
                QString pName = QString("P_%0_%1%2").arg(name).arg(i).arg(j);
                if (IsPipe)
                {
                    QVector3D pos = QVector3D(Width * cos(2 * M_PIf * j / w), Width * sin(2 * M_PIf * j / w),
                                              (float) i / (PATCH_SIZE - 1) * Height / V);
                    p = std::make_shared<Point>(pName, pos + m_transform->Position);
                    p->p_Transform->Locked = true;
                } else
                    p = std::make_shared<Point>(pName, QVector3D((float) j / (PATCH_SIZE - 1) * Width / U, 0,
                                                                 (float) i / (PATCH_SIZE - 1) * Height / V) +
                                                       m_transform->Position);
                res.emplace_back(p);
                scene->AddObjectExplicitPosition(p);
            }
    }

    return res;
}
