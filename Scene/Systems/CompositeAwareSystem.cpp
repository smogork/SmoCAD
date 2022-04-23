//
// Created by ksm on 4/9/22.
//

#include "CompositeAwareSystem.h"
#include "Scene/Entities/Composite.h"
#include "Scene/SceneECS.h"

unsigned int CompositeAwareSystem::SelectMultipleObjects(std::list<unsigned int> objectIds)
{
    std::list<CompositeAware> elements;
    std::shared_ptr<Composite> composite;
    for (unsigned int oid : objectIds)
    {
        if (auto ca = GetComponent(oid).lock())
        {
            if (composite)
                composite->AddObject(ca);
            else
                composite = std::make_shared<Composite>(ca);
        }
    }

    if (composite)
    {

        compositeOutOfContextNotifier = composite->p_Selectable->Selected.addNotifier([this]()
            {
                if (!currentComposite->p_Selectable->Selected)
                    currentComposite.reset();
            });
        currentComposite = composite;
        //podlaczenie kompozytu jako aktualnie wybrany obiekt
        composite->p_Selectable->Selected = true;
        return composite->GetObjectID();
    }
    return SceneECS::NON_OBJECT_ID;
}

void CompositeAwareSystem::ClearSystem()
{
    currentComposite.reset();
}
