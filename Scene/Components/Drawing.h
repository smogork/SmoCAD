//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWING_H
#define SMOCAD_DRAWING_H


#include <QOpenGLVertexArrayObject>
#include "IComponent.h"
#include "Renderer/ShaderWrapper.h"

class Drawing: public IComponent
{
private:
    std::shared_ptr<ShaderWrapper> shader = nullptr;
    std::shared_ptr<QOpenGLVertexArrayObject> va = nullptr;
    std::function<void(void)> renderingFunction = {};

public:

    static std::shared_ptr<Drawing> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    Drawing(unsigned int oid);
    ~Drawing() override;

    void Render();
};


#endif //SMOCAD_DRAWING_H
