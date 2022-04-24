//
// Created by ksm on 4/3/22.
//

#include "Drawing.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"


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

void Drawing::AttachShader(std::shared_ptr<ShaderWrapper> shader)
{
    m_shader = shader;
}

void Drawing::SetIndexData(std::vector<int> data)
{
    m_indexArrayData = data;
}

void Drawing::SetVertexData(std::vector<float> data)
{
    m_vertexArrayData = data;
}

std::vector<float> Drawing::GetVertex(int index, int indexStride)
{
   std::vector<float> res(indexStride);

   int idx = (index) * indexStride;
   for (int i = 0; i < indexStride; ++i)
       res[i] = m_vertexArrayData[idx + i];
   return res;
}
