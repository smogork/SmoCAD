//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCENEELEMENT_H
#define SMOCAD_SCENEELEMENT_H


#include "IComponent.h"
#include "Selectable.h"

class SceneElement: public IComponent
{
private:

public:
    QProperty<QString> Name;
    std::shared_ptr<Selectable> p_Selected;

    static std::shared_ptr<SceneElement> CreateRegisteredComponent(unsigned int oid,
        const QString& name, std::shared_ptr<Selectable> select);
    void UnregisterComponent();

    explicit SceneElement(unsigned int oid);
    ~SceneElement() override;
};


#endif //SMOCAD_SCENEELEMENT_H
