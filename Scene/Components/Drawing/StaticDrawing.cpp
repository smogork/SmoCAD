//
// Created by ksm on 4/5/22.
//

#include "StaticDrawing.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"

void StaticDrawing::Render(QOpenGLContext* context)
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

void StaticDrawing::IntializeBuffers()
{
    m_vbo->create();
    m_vbo->bind();
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo->allocate(m_vertexArrayData.data(), sizeof(float) * m_vertexArrayData.size());
    m_vbo->release();

    m_ibo->create();
    m_ibo->bind();
    m_ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo->allocate(m_indexArrayData.data(), sizeof(int) * m_indexArrayData.size());
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

StaticDrawing::StaticDrawing(unsigned int oid) : Drawing(oid)
{

}

std::shared_ptr<StaticDrawing> StaticDrawing::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
        {
            std::shared_ptr<StaticDrawing> res = std::make_shared<StaticDrawing>(oid);
            system->RegisterComponent(res);
            return res;
        }
    }
    return nullptr;
}
