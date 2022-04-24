//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_VIRTUALPOINT_H
#define SMOCAD_VIRTUALPOINT_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"
#include "Scene/Components/CollectionAware.h"
#include "Scene/Components/Selectable.h"
#include "Scene/Components/ScreenSelectable.h"

class VirtualPoint: public IEntity
{
private:
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    QVector4D m_color = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    QPropertyNotifier selectedNotifier;

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    std::shared_ptr<Selectable> p_Selectable;
    std::shared_ptr<ScreenSelectable> p_ScreenSelectable;
    std::shared_ptr<CollectionAware> p_CollectionAware;

    explicit VirtualPoint(QVector3D pos);
};


#endif //SMOCAD_VIRTUALPOINT_H
