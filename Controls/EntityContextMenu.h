//
// Created by ksm on 6/12/22.
//

#ifndef SMOCAD_ENTITYCONTEXTMENU_H
#define SMOCAD_ENTITYCONTEXTMENU_H

#include <QMenu>

class EntityContextMenu: public QObject
{
    Q_OBJECT
private:
    EntityContextMenu();
    static std::shared_ptr<EntityContextMenu> instance;
    
    void EmitRepaint();
    void EmitControlsUpdate(unsigned int oid);
    
public:
    static std::unique_ptr<QMenu> CreateMenuForScene();
    static std::unique_ptr<QMenu> CreateMenuForSceneList();
    
    static void MakeRepaint();
    static void MakeControlsUpdate(unsigned int oid);
    static std::shared_ptr<EntityContextMenu> GetInstance();

signals:
    void RequestRepaint();
    void RequestControlsUpdate(unsigned int oid);
};

#endif //SMOCAD_ENTITYCONTEXTMENU_H
