//
// Created by ksm on 5/17/22.
//

#include "PlaneCreator.h"
#include "Scene/SceneECS.h"
#include "Mathematics/PointShapes.h"

PlaneCreator::PlaneCreator(const QString &name, uint creatingCid, QVector3D pos) : IEntity(PLANE_CREATOR_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    AddComponent(p_UVParams = UVPlaneCreator::CreateRegisteredComponent(GetObjectID(), p_Transform, creatingCid, 4, 3));
    p_UVParams->UDensity = 4;
    p_UVParams->VDensity = 4;

    m_mesh.p_Collection->SecondDimension = p_UVParams->U + 1;

    uNotifier = p_UVParams->U.addNotifier([this]()
                                          {
                                                if (p_UVParams->IsPipe)
                                                    m_mesh.p_Collection->SecondDimension = p_UVParams->U;
                                                else
                                                    m_mesh.p_Collection->SecondDimension = p_UVParams->U + 1;
                                              CreateTempMesh();
                                          });
    vNotifier = p_UVParams->V.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });
    wNotifier = p_UVParams->Width.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });
    hNotifier = p_UVParams->Height.addNotifier([this]()
                                          {
                                              CreateTempMesh();
                                          });
    pipeNotifier = p_UVParams->IsPipe.addNotifier([this]()
                                               {
                                                   if (p_UVParams->IsPipe)
                                                       m_mesh.p_Collection->SecondDimension = p_UVParams->U;
                                                   else
                                                       m_mesh.p_Collection->SecondDimension = p_UVParams->U + 1;
                                                   m_mesh.IsPipe = p_UVParams->IsPipe;
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

void PlaneCreator::CreateTempMesh()
{
    m_mesh.p_Collection->Clear();
    if (p_UVParams->IsPipe)
        CreatePoints(p_UVParams->U, p_UVParams->V + 1);
    else
        CreatePoints(p_UVParams->U + 1, p_UVParams->V + 1);
    m_mesh.p_Collection->SetPoints(elements);
}

void PlaneCreator::CreatePoints(int w, int h)
{
    elements.clear();
    points.clear();

    std::vector<QVector3D> positions;
    if (p_UVParams->IsPipe)
        positions = PointShapes::CreateTube(p_Transform->Position, p_UVParams->Width, p_UVParams->Height, w, h);
    else
        positions = PointShapes::CreateRect(p_Transform->Position, p_UVParams->Width, p_UVParams->Height, w, h);

    for (const auto& t : positions)
    {
        auto p = std::make_shared<VirtualPoint>(t);
        p->p_Transform->Locked = true;
        points.push_back(p);
        elements.emplace_back(p->p_CollectionAware);
    }
}
