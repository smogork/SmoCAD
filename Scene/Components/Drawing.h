//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWING_H
#define SMOCAD_DRAWING_H


#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLContext>

#include "IComponent.h"
#include "Renderer/ShaderWrapper.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"

#define ASSIGN_DRAWING_FUNCTION(F)  std::bind(F, this, std::placeholders::_1, std::placeholders::_2)

class Drawing: public IComponent
{
private:
    std::shared_ptr<ShaderWrapper> m_shader = nullptr;
    std::unique_ptr<QOpenGLBuffer> m_vbo = nullptr;
    std::unique_ptr<QOpenGLBuffer> m_ibo = nullptr;
    std::shared_ptr<QOpenGLVertexArrayObject> m_vao = nullptr;
    bool m_created = false;

protected:
    void IntializeBuffers();

public:
    std::vector<float> p_vertexArrayData;
    std::vector<int> p_indexArrayData;
    BufferLayout p_bufferLayout;
    std::function<void(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader)> p_renderingFunction = {};

    static std::shared_ptr<Drawing> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    Drawing(unsigned int oid);
    ~Drawing() override;


    void AttachShader(std::shared_ptr<ShaderWrapper> shader);
    void Render(QOpenGLContext* context);
};


#endif //SMOCAD_DRAWING_H
