//
// Created by ksm on 4/5/22.
//

#include "DynamicDrawing.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/SceneECS.h"

void DynamicDrawing::Render(QOpenGLContext* context)
{
    if (m_toUpdate)
    {
        m_toUpdate = false;
        UpdateBuffers();
    }

    StaticDrawing::Render(context);
}

void DynamicDrawing::UpdateBuffers()
{
    if (!m_created)
        return;

    m_vbo->bind();
    m_vbo->allocate(m_vertexArrayData.data(), sizeof(float) * m_vertexArrayData.size());
    m_vbo->release();

    m_ibo->bind();
    m_ibo->allocate(m_indexArrayData.data(), sizeof(int) * m_indexArrayData.size());
    m_ibo->release();
}

DynamicDrawing::DynamicDrawing(unsigned int oid) : StaticDrawing(oid)
{
    usage = QOpenGLBuffer::DynamicDraw;
}

std::shared_ptr<DynamicDrawing> DynamicDrawing::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
        {
            std::shared_ptr<DynamicDrawing> res = std::make_shared<DynamicDrawing>(oid);
            system->RegisterComponent(res);
            return res;
        }
    }
    return nullptr;
}

void DynamicDrawing::SetVertexData(std::vector<float> data)
{
    m_toUpdate = true;
    Drawing::SetVertexData(data);
}

void DynamicDrawing::SetIndexData(std::vector<int> data)
{
    m_toUpdate = true;
    Drawing::SetIndexData(data);
}
