//
// Created by ksm on 5/17/22.
//

#include "PlaneCreator.h"
#include "Scene/SceneECS.h"

PlaneCreator::PlaneCreator(const QString &name, QVector3D pos) : IEntity(PLAIN_CREATOR_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(objectID, pos));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(objectID));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected));
    AddComponent(p_UVParams = UVPlaneCreator::CreateRegisteredComponent(objectID, 4, 3));
    p_UVParams->UDensity = 7;
    p_UVParams->VDensity = 4;

    m_mesh.p_Collection->SecondDimension = p_UVParams->U;

    uNotifier = p_UVParams->U.addNotifier([this]()
    {
        m_mesh.p_Collection->SecondDimension = p_UVParams->U;
        CreateTempMesh();
    });
    vNotifier = p_UVParams->V.addNotifier([this]()
    {
      CreateTempMesh();
    });

    posNotifier = p_Transform->Position.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });
    rotNotifier = p_Transform->Rotation.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });
    scaleNotifier = p_Transform->Scale.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });

    CreateTempMesh();
}

std::list<std::shared_ptr<VirtualPoint>>
PlaneCreator::GetPatchPoints(std::list<std::shared_ptr<VirtualPoint>> points, int i, int j)
{
    return std::list<std::shared_ptr<VirtualPoint>>();
}

void PlaneCreator::CreateTempMesh()
{
    m_mesh.p_Collection->Clear();
    CreatePoints(p_UVParams->U, p_UVParams->V);
    m_mesh.p_Collection->SetPoints(elements);
}

void PlaneCreator::CreatePoints(int w, int h, Plane p)
{
    elements.clear();
    points.clear();

    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
            {
                auto p = std::make_shared<VirtualPoint>(QVector3D((PATCH_SIZE - 1) * i, 0, (PATCH_SIZE - 1) * j)
                        + p_Transform->Position);
                points.push_back(p);
                elements.emplace_back(p->p_CollectionAware);
                //scene->AddObjectExplicitPosition(p);
            }
    }

    //to kawalek do tworzenia legitnej siatki dla prawdizwej plaszczyzny
    /*if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < (PATCH_SIZE - 1) * h + 1; ++i)
            for (int j = 0; j < (PATCH_SIZE - 1) * w + 1; ++j)
            {
                auto p = std::make_shared<VirtualPoint>(QString("P_%0_%1%2").arg(p_SceneElement->Name).arg(i).arg(j),
                                                 QVector3D(i, 0, j) + p_Transform->Position);
                points.emplace_back(p);
                scene->AddObjectExplicitPosition(p);
            }
    }*/

}
