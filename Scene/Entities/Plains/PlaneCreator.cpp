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
    AddComponent(p_UVParams = UVPlaneCreator::CreateRegisteredComponent(objectID, p_Transform, 4, 3));
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

void PlaneCreator::CreatePoints(int w, int h, Plane p)
{
    elements.clear();
    points.clear();

    if (auto scene = SceneECS::Instance().lock())
    {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
            {
                QVector3D pos;
                if (p_UVParams->IsPipe)
                    pos = QVector3D(p_UVParams->Width * cos(2 * M_PIf * j / w),
                                              p_UVParams->Width * sin(2 * M_PIf * j / w),
                                    (float)i * p_UVParams->Height / p_UVParams->V);
                else
                    pos = QVector3D((float)j * p_UVParams->Width / p_UVParams->U, 0, (float)i * p_UVParams->Height / p_UVParams->V);

                auto p = std::make_shared<VirtualPoint>(pos + p_Transform->Position);
                p->p_Transform->Locked = true;

                points.push_back(p);
                elements.emplace_back(p->p_CollectionAware);
            }
    }
}
