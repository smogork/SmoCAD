//
// Created by ksm on 6/12/22.
//

#include "EntityContextMenu.h"
#include "Scene/SceneECS.h"

std::shared_ptr<EntityContextMenu> EntityContextMenu::instance = nullptr;

EntityContextMenu::EntityContextMenu()
{

}

std::unique_ptr<QMenu> EntityContextMenu::CreateMenuForScene()
{
    std::unique_ptr<QMenu> myMenu = std::make_unique<QMenu>();
    
    if (auto scene = SceneECS::Instance().lock())
    {
        auto menu_items = scene->GenerateContextMenuItemsForScene();
        if (menu_items.empty())
            return nullptr;
        
        auto selectedObjs = scene->GetSelectedObjects();
        
        for (const std::pair<QString, std::function<void(
                    const std::vector<unsigned int> &selectedOids)> > &menu_item: menu_items)
        {
            const std::function<void(
                    const std::vector<unsigned int> &selectedOids)> &func = menu_item.second;
            myMenu->addAction(menu_item.first, GetInstance().get(), [func, selectedObjs]()
            {
                if (func)
                    func(selectedObjs);
            });
        }
    }
    
    return myMenu;
}

std::unique_ptr<QMenu> EntityContextMenu::CreateMenuForSceneList()
{
    std::unique_ptr<QMenu> myMenu = std::make_unique<QMenu>();
    
    if (auto scene = SceneECS::Instance().lock())
    {
        auto menu_items = scene->GenerateContextMenuItemsForSceneList();
        if (menu_items.empty())
            return nullptr;
        
        auto selectedObjs = scene->GetSelectedObjects();
        auto contextObjs = scene->GetListContextObjects();
        
        for (const std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                         const std::vector<unsigned int> &listContextOids)> > &menu_item: menu_items)
        {
            const std::function<void(const std::vector<unsigned int> &selectedOids,
                                     const std::vector<unsigned int> &listContextOids)> &func = menu_item.second;
            myMenu->addAction(menu_item.first, GetInstance().get(), [func, selectedObjs, contextObjs]()
            {
                if (func)
                    func(selectedObjs, contextObjs);
            });
        }
    }
    
    return myMenu;
}

void EntityContextMenu::MakeRepaint()
{
    GetInstance()->EmitRepaint();
    
}

void EntityContextMenu::MakeControlsUpdate(unsigned int oid)
{
    GetInstance()->EmitControlsUpdate(oid);
}

std::shared_ptr<EntityContextMenu> EntityContextMenu::GetInstance()
{
    if (!instance)
        instance = std::shared_ptr<EntityContextMenu>(new EntityContextMenu);
    return instance;
}

void EntityContextMenu::EmitRepaint()
{
    emit RequestRepaint();
}

void EntityContextMenu::EmitControlsUpdate(unsigned int oid)
{
    emit RequestControlsUpdate(oid);
}
