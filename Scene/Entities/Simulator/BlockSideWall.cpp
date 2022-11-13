//
// Created by ksm on 10/21/22.
//

#include "BlockSideWall.h"
#include "Mathematics/PointShapes.h"
#include <QMatrix4x4>

BlockSideWall::BlockSideWall(QVector3D pos, std::shared_ptr<Transform> simulatorTransform,
                             std::shared_ptr<QOpenGLTexture> heightMap, double widthX, double widthY, double height,
                             int vertexWidthX, int vertexWidthY)
        : IEntity(SIMULATOR_BLOCK_SIDE), m_widthX(widthX), m_widthY(widthY), m_height(height),
          m_vertexWidthX(vertexWidthX), m_vertexWidthY(vertexWidthY), m_simulatorTransform(simulatorTransform),
          m_heightTexture(heightMap)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(GetObjectID()));
    InitializeDrawing();
}

void BlockSideWall::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->SetIndexData(GenerateTopologyIndices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    p_Drawing->p_bufferLayout.Push<float>(2);//texCoord
    p_Drawing->p_bufferLayout.Push<float>(1);//ignoreHeight
    if (auto sh = Renderer::GetShader(BLOCK_SIDEWALL_SHADER).lock())
        p_Drawing->AttachShader(sh);
    
    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BlockSideWall::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BlockSideWall::UniformFunction);
}

void BlockSideWall::DrawingFunction(QOpenGLContext *context)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
    Renderer::DrawTriangles(context->functions(), m_indexCount);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
}

void BlockSideWall::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    QMatrix4x4 modelMtx = p_Transform->GetModelMatrix();
    if (auto trans = m_simulatorTransform.lock())
        modelMtx = trans->GetModelMatrix() * modelMtx;
    shader->SetUniform("u_MVP.Model", modelMtx);
    
    m_heightTexture->bind(0);
    shader->SetUniform("heightTexture", 0);
    shader->SetUniform("u_MaxHeight", (float)m_height);
}

std::vector<float> BlockSideWall::GenerateGeometryVertices()
{
    QMatrix4x4 rot;
    rot.rotate(180, Transform::GetYAxis());
    
    std::vector<QVector3D> wallA = PointShapes::CreateRect(QVector3D(-m_widthX / 2.0f, 0.0f, -m_widthY / 2.0f),
                                                           m_widthX, m_height, m_vertexWidthX, 2, XY);
    std::vector<QVector3D> wallC = PointShapes::CreateRect(QVector3D(-m_widthX / 2.0f, 0.0f, -m_widthY / 2.0f),
                                                           m_widthY, m_height, m_vertexWidthY, 2, YZ);
    
    std::vector<QVector3D> wallB(wallA.size()), wallD(wallC.size());
    auto rotationFunc = [rot](const QVector3D &p)
    {
        return (rot * p.toVector4D()).toVector3D();
    };
    
    std::transform(wallA.begin(), wallA.end(), wallB.begin(), rotationFunc);
    std::transform(wallC.begin(), wallC.end(), wallD.begin(), rotationFunc);
    
    std::vector<float> res;
    CreateVerticesForWall(wallA, m_vertexWidthX, res, 1, 0);
    CreateVerticesForWall(wallB, m_vertexWidthX, res, -1, 2);
    CreateVerticesForWall(wallC, m_vertexWidthY, res, 0, 1);
    CreateVerticesForWall(wallD, m_vertexWidthY, res, 2, -1);
    
    return res;
}

std::vector<int> BlockSideWall::GenerateTopologyIndices()
{
    std::vector<int> retX = PointShapes::RectTriangleIndices(m_vertexWidthX, 2);
    std::vector<int> retX2 = PointShapes::RectTriangleIndices(m_vertexWidthX, 2);
    std::vector<int> retY = PointShapes::RectTriangleIndices(m_vertexWidthY, 2, false);
    std::vector<int> retY2 = PointShapes::RectTriangleIndices(m_vertexWidthY, 2, false);
    int offset = m_vertexWidthX * 2;
    std::for_each(retX2.begin(), retX2.end(), [offset](auto &el) { el += offset; });
    offset += m_vertexWidthX * 2;
    std::for_each(retY.begin(), retY.end(), [offset](auto &el) { el += offset; });
    offset += m_vertexWidthY * 2;
    std::for_each(retY2.begin(), retY2.end(), [offset](auto &el) { el += offset; });
    
    retX.insert(retX.end(), retX2.begin(), retX2.end());
    retX.insert(retX.end(), retY.begin(), retY.end());
    retX.insert(retX.end(), retY2.begin(), retY2.end());
    m_indexCount = retX.size();
    return retX;
}

void BlockSideWall::CreateVerticesForWall(const std::vector<QVector3D> &wall, int vertexWidth, std::vector<float> &res,
                                          int scaleU, int scaleV)
{
    for (int h = 0; h < 2; h++)
        for (int i = 0; i < vertexWidth; ++i)
        {
            res.push_back(wall[h * vertexWidth + i].x());
            res.push_back(wall[h * vertexWidth + i].y());
            res.push_back(wall[h * vertexWidth + i].z());
            res.push_back(ScaleTexCoord(i, vertexWidth, scaleU));
            res.push_back(ScaleTexCoord(i, vertexWidth, scaleV));
            res.push_back(h == 0 ? -1.0f : 1.0f);
        }
}

float BlockSideWall::ScaleTexCoord(int vertexNum, int vertexCount, int scaleFlag)
{
    switch (scaleFlag)
    {
        case 1:
            return (float) vertexNum / (vertexCount - 1);
        case 2:
            return 1.0;
        case -1:
            return 1.0f - (float) vertexNum / (vertexCount - 1);
        case 0:
        default:
            return 0;
    }
}
