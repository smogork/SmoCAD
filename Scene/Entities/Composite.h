//
// Created by ksm on 4/8/22.
//

#ifndef SMOCAD_COMPOSITE_H
#define SMOCAD_COMPOSITE_H


#include "Scene/Entities/IEntity.h"
#include "Scene/Entities/Cursor.h"
#include "Scene/Components/CompositeAware.h"
#include "Scene/Components/Selectable.h"

class Composite: public IEntity
{
    Q_OBJECT

    class CompositeElement: IEntity
    {
    public:
        unsigned int servingObjectID;
        std::weak_ptr<CompositeAware> element;
        std::shared_ptr<Transform> dTransform;
        std::shared_ptr<Transform> compositeTransform;

        CompositeElement(Composite* composite, std::shared_ptr<CompositeAware> el);
        ~CompositeElement() override;

        void UpdateDTransform();
        void UpdateServingObject();
    };
private:
    std::unique_ptr<Cursor> m_center;
    std::list<std::unique_ptr<CompositeElement>> objects;
    QPropertyNotifier compositePositionNotifier;
    QPropertyNotifier compositeRotationNotifier;
    QPropertyNotifier compositeScaleNotifier;

    void UpdateCompositeElements();
public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<Selectable> p_Selectable;

    explicit Composite(std::shared_ptr<CompositeAware> startObject);
    ~Composite() override;

    void AddObject(std::shared_ptr<CompositeAware> newObject);

};


#endif //SMOCAD_COMPOSITE_H
