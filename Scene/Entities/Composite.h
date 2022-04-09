//
// Created by ksm on 4/8/22.
//

#ifndef SMOCAD_COMPOSITE_H
#define SMOCAD_COMPOSITE_H


#include "Scene/Entities/IEntity.h"
#include "Scene/Entities/Cursor.h"
#include "Scene/Components/CompositeAware.h"

class Composite: public IEntity
{
    Q_OBJECT

    class CompositeElement: IEntity
    {
    private:
        void DecoratingUniformFunction(std::shared_ptr<ShaderWrapper> shader);

    public:
        unsigned int servingObjectID;
        std::shared_ptr<Drawing> objDrawing;
        std::shared_ptr<Transform> objTransform;
        std::shared_ptr<Transform> dTransform;
        std::shared_ptr<Transform> compositeTransform;
        std::function<void(std::shared_ptr<ShaderWrapper> shader)> originalUniform;

        CompositeElement(Composite* composite, std::shared_ptr<Transform> objTransform, std::shared_ptr<Drawing> objDrawing);
        ~CompositeElement();
    };
private:
    std::unique_ptr<Cursor> m_center;
    std::list<std::unique_ptr<CompositeElement>> objects;

    void SetPosition(QVector3D pos);

public:
    std::shared_ptr<Transform> p_Transform;

    explicit Composite(std::shared_ptr<CompositeAware> startObject);
    ~Composite() override;

    void AddObject(std::shared_ptr<CompositeAware> newObject);

};


#endif //SMOCAD_COMPOSITE_H
