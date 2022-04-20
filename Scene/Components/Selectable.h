//
// Created by ksm on 4/7/22.
//

#ifndef SMOCAD_SELECTABLE_H
#define SMOCAD_SELECTABLE_H


#include "IComponent.h"
#include "Transform.h"
#include "Scene/Events/SystemEvents/SelectionChanged.h"

#include <QVector4D>

class Selectable: public IComponent
{
    Q_OBJECT
private:
public:
    QProperty<bool> Selected;

    static std::shared_ptr<Selectable> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    explicit Selectable(unsigned int oid);
    ~Selectable() override;
};


#endif //SMOCAD_SELECTABLE_H
