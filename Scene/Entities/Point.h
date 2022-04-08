//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_POINT_H
#define SMOCAD_POINT_H


#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/Drawing.h"
#include "Scene/Components/Drawing/StaticDrawing.h"
#include "Scene/Components/Selectable.h"

class Point: public IEntity
{
private:
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    std::shared_ptr<Selectable> p_Selectable;

    explicit Point(QVector3D pos);
    ~Point() override;
};


#endif //SMOCAD_POINT_H
