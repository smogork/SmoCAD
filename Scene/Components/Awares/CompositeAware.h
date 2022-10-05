//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COMPOSITEAWARE_H
#define SMOCAD_COMPOSITEAWARE_H


#include "Scene/Components/IComponent.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/Drawing.h"

class CompositeAware: public IComponent
{
public:
    static const QColor CompositeColor;
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<Drawing> p_Drawing;
    QProperty<bool> InsideComposite;

    static std::shared_ptr<CompositeAware> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> t, std::shared_ptr<Drawing> d);
    void UnregisterComponent();

    explicit CompositeAware(unsigned int oid);
    ~CompositeAware() override;

    //CreateCompositeElement?
};


#endif //SMOCAD_COMPOSITEAWARE_H
