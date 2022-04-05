//
// Created by ksm on 4/3/22.
//

#include "Drawing.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"

std::shared_ptr<Drawing> Drawing::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
            return system->CreateRegistered(oid);
    }
    return nullptr;
}

void Drawing::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
            system->Unregister(GetAttachedObjectID());
    }
}

Drawing::~Drawing()
{
    UnregisterComponent();

    m_vao->destroy();
    m_vbo->destroy();
    m_ibo->destroy();
}

Drawing::Drawing(unsigned int oid): IComponent(oid, SYSTEM_ID::DRAWING)
{
    m_vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    m_ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
}

void Drawing::Render(QOpenGLContext* context)
{
    if (!m_created)
    {
        m_created = true;
        IntializeBuffers();
    }

    m_vao->bind();
    if (p_renderingFunction)
        p_renderingFunction(context, m_shader);
    m_vao->release();
}

void Drawing::IntializeBuffers()
{
    m_vbo->create();
    m_vbo->bind();
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo->allocate(p_vertexArrayData.data(), sizeof(float) * p_vertexArrayData.size());
    m_vbo->release();

    m_ibo->create();
    m_ibo->bind();
    m_ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo->allocate(p_indexArrayData.data(), sizeof(int) * p_indexArrayData.size());
    m_ibo->release();

    //To jest fake m_shader, który oszukuje qt wrapper na opengl
    //atrybuty nie są zwaizane ze stane shadera, tylko vertex array.
    //Wiec tworzymy sztuczny obiekt shadera aby zadeklarowac w VA uklad atrybutow.
    QOpenGLShaderProgram prog;
    prog.create();
    m_vao->create();
    prog.bind();
    m_vao->bind();
    m_vbo->bind();

    const auto& elements = p_bufferLayout.GetElements();
    int offset = 0;
    for (int i = 0; i < elements.size(); ++i)
    {
        const auto& element = elements[i];
        prog.enableAttributeArray(0);
        prog.setAttributeBuffer(i, element.type, offset, element.count, p_bufferLayout.GetStride());
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

    m_ibo->bind();
    m_vao->release();

    m_vbo->release();
    m_ibo->release();
    prog.release();
}

void Drawing::AttachShader(std::shared_ptr<ShaderWrapper> shader)
{
    m_shader = shader;
}
