//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWING_H
#define SMOCAD_DRAWING_H


#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLContext>

#include "Scene/Components/IComponent.h"
#include "Renderer/ShaderWrapper.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"

//https://stackoverflow.com/a/7582576/18323924
#define ASSIGN_DRAWING_FUNCTION(F)  std::bind(F, this, std::placeholders::_1, std::placeholders::_2)

class Drawing: public IComponent
{
    Q_OBJECT
protected:
    std::shared_ptr<ShaderWrapper> m_shader = nullptr;
    std::unique_ptr<QOpenGLBuffer> m_vbo = nullptr;
    std::unique_ptr<QOpenGLBuffer> m_ibo = nullptr;
    std::shared_ptr<QOpenGLVertexArrayObject> m_vao = nullptr;
    bool m_created = false;
    virtual void IntializeBuffers() = 0;

    std::vector<float> m_vertexArrayData;
    std::vector<int> m_indexArrayData;

public:

    BufferLayout p_bufferLayout;
    std::function<void(QOpenGLContext* context, std::shared_ptr<ShaderWrapper> shader)> p_renderingFunction = {};

    //static std::shared_ptr<Drawing> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    Drawing(unsigned int oid);
    ~Drawing() override;

    void AttachShader(std::shared_ptr<ShaderWrapper> shader);
    virtual void Render(QOpenGLContext* context) = 0;

    virtual void SetVertexData(std::vector<float> data);
    virtual void SetIndexData(std::vector<int> data);
};


#endif //SMOCAD_DRAWING_H