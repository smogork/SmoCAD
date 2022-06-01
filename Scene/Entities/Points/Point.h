//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_POINT_H
#define SMOCAD_POINT_H


#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/Drawing.h"
#include "Scene/Components/Drawing/StaticDrawing.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/CompositeAware.h"
#include "Scene/Components/CollectionAware.h"
#include "Scene/Components/ScreenSelectable.h"
#include "Scene/Components/SceneElement.h"

class Point: public IEntity
{
private:
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    QColor m_color = DefaultColor;
    QPropertyNotifier selectedNotifier;
    QPropertyNotifier compositeNotifier;

    void HandleColors();
    void SerializingFunction(MG1::Scene& scene);

    void InitObject(const QString &name, QVector3D pos);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    std::shared_ptr<Selectable> p_Selectable;
    std::shared_ptr<ScreenSelectable> p_ScreenSelectable;
    std::shared_ptr<CompositeAware> p_CompositeAware;
    std::shared_ptr<CollectionAware> p_CollectionAware;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit Point(const QString& name);
    explicit Point(const MG1::Point& serializedObj);
    Point(const QString& name, QVector3D pos);
};


#endif //SMOCAD_POINT_H
