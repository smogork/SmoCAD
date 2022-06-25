//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONAWARESYSTEM_H
#define SMOCAD_INTERSECTIONAWARESYSTEM_H

#include "Scene/Components/Awares/IntersectionAware.h"
#include "Scene/Systems/ISystem.h"

class IntersectionAwareSystem : public ISystem<IntersectionAware>
{
public:
    IntersectionAwareSystem() : ISystem(SYSTEM_ID::INTERSECTION_AWARE)
    {}

    const char *GetSystemName() override
    { return "IntersectionAwareSystem"; }

    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids) override;

protected:
    void CreateIntersectionCurveBetween(std::shared_ptr<IntersectionAware> one, std::shared_ptr<IntersectionAware> two);
    QVector4D
    FindFirstPointOfIntersection(std::shared_ptr<IntersectionAware> one, std::shared_ptr<IntersectionAware> two);
    std::list<QVector4D>
    FindFurtherPointsOfIntersection(QVector4D P0, float dist, bool direction, bool& edgeEnd, std::shared_ptr<IntersectionAware> one,
                                    std::shared_ptr<IntersectionAware> two);

};

#endif //SMOCAD_INTERSECTIONAWARESYSTEM_H
