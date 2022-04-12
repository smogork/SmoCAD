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
    std::shared_ptr<Transform> m_transform = nullptr;
    //[TODO] przerobic na QProperty
    bool m_selected = false;

signals:
    void SelectedChanged(std::shared_ptr<SelectionChanged> e);

public:
    static std::shared_ptr<Selectable> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform);
    void UnregisterComponent();

    explicit Selectable(unsigned int oid);
    ~Selectable() override;

    float TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection, float cameraPivotLength);
    void SetSelection(bool val);
    bool Selected();
};


#endif //SMOCAD_SELECTABLE_H
