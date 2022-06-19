//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCENEELEMENT_H
#define SMOCAD_SCENEELEMENT_H


#include "IComponent.h"
#include "Selectable.h"

#include "Scene/Scene.h"
#include "Scene/Components/Awares/CompositeAware.h"

#define ASSIGN_SERIALIZER_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)

class SceneElement: public IComponent
{
private:

public:
    QProperty<QString> Name;
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<CompositeAware> p_CompositeAware;

    static std::shared_ptr<SceneElement> CreateRegisteredComponent(unsigned int oid,
        const QString& name, std::shared_ptr<Selectable> select, std::shared_ptr<CompositeAware> composite = nullptr);
    void UnregisterComponent();
    std::function<void(MG1::Scene& scene)> SerializeObject = {};

    explicit SceneElement(unsigned int oid);
    ~SceneElement() override;
};


#endif //SMOCAD_SCENEELEMENT_H
