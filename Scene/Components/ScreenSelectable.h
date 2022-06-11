//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCREENSELECTABLE_H
#define SMOCAD_SCREENSELECTABLE_H

#include "IComponent.h"
#include "Transform.h"
#include "Selectable.h"
#include "CompositeAware.h"

class ScreenSelectable : public IComponent
{
Q_OBJECT

private:
    std::shared_ptr<Transform> m_transform = nullptr;

public:
    std::shared_ptr<Selectable> p_Selected = nullptr;
    std::shared_ptr<CompositeAware> p_CompositeAware = nullptr;
    
    static std::shared_ptr<ScreenSelectable> CreateRegisteredComponent(unsigned int oid,
                                                                       std::shared_ptr<Transform> transform,
                                                                       std::shared_ptr<Selectable> select,
                                                                       std::shared_ptr<CompositeAware> composite = nullptr);
    void UnregisterComponent();
    
    explicit ScreenSelectable(unsigned int oid);
    ~ScreenSelectable() override;
    
    float TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection, float cameraPivotLength);
    bool IsInsideRectangle(QRect selectRectangle);
    bool CanBeInsideComposite();
};

#endif //SMOCAD_SCREENSELECTABLE_H
