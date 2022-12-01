//
// Created by ksm on 5/17/22.
//

#ifndef SMOCAD_UVPLANECREATOR_H
#define SMOCAD_UVPLANECREATOR_H

#include "IComponent.h"
#include "Transform.h"
#include "Scene/Entities/Planes/PlaneC0.h"
#include "Scene/SceneECS.h"

#define PATCH_SIZE 4

class UVPlaneCreator : public IComponent
{
Q_OBJECT

private:
    std::shared_ptr<Transform> m_transform = nullptr;

public:
    QProperty<int> U;
    QProperty<int> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;
    QProperty<double> Width, Height;
    QProperty<bool> IsPipe;
    uint m_creatingCid;

    explicit UVPlaneCreator(unsigned int oid);
    ~UVPlaneCreator() override;

    static std::shared_ptr<UVPlaneCreator>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, uint cid, int U, int V);
    void UnregisterComponent();
    uint GetCID() { return m_creatingCid; }

    template <typename P>
    std::shared_ptr<P> CreatePlane(const QString &name)
    {
        //Utworz punkty budujace plaszczyzne i dodaj je do sceny
        auto points = P::CreatePointsForPlane(m_transform->Position, name, IsPipe, U, V, Width, Height);

        //Dodaj punkty w odpowiedniej kolejnosci do plaszczyzny
        auto plane = std::make_shared<P>(name, IsPipe, U, V);

        std::vector<std::shared_ptr<CollectionAware>> elems(points.size());
        for (int i = 0; i < elems.size(); ++i)
            elems[i] = points[i]->p_CollectionAware;
        plane->p_Collection->SetPoints(elems);
        plane->p_Collection->LockContent();

        //przenies parametry UV do plaszczyzny
        plane->p_UV->UDensity = *UDensity;
        plane->p_UV->VDensity = *VDensity;
        plane->p_UV->LockEditUV();

        return plane;
    }
};

#endif //SMOCAD_UVPLANECREATOR_H
