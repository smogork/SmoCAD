//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_POINT_H
#define SMOCAD_POINT_H


#include "IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing.h"

class Point: public IEntity
{
public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<Drawing> p_Drawing;

    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader);

    explicit Point(QVector3D pos);
    ~Point() override;
};


#endif //SMOCAD_POINT_H
