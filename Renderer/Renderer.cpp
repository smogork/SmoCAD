//
// Created by ksm on 4/5/22.
//

#include "Renderer.h"

std::map<int, std::shared_ptr<ShaderWrapper>> Renderer::shaders;
InputController Renderer::controller(std::make_shared<Viewport>(QSize(), 60));
QProperty<double> Renderer::EyeSeparation(0.1f);

void Renderer::DrawTriangles(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawLines(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPatches(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawElements(GL_PATCHES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawLineStrip(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawArrays(GL_LINE_STRIP, 0, count);
}

void Renderer::DrawPoints(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0);
}

void Renderer::AddShader(SHADERS id, std::shared_ptr<ShaderWrapper> sh)
{
    shaders.insert(std::make_pair(id, sh));
}

std::weak_ptr<ShaderWrapper> Renderer::GetShader(SHADERS id)
{
    auto it = shaders.find(id);
    if (it == shaders.end())
        throw std::runtime_error(QString("Cannot find shader with %1 id").arg(id).toStdString());
    return it->second;
}

void Renderer::DeleteShaders()
{
    shaders.clear();
}

void Renderer::UpdateShaders()
{
    for (auto sh: shaders)
    {
        std::shared_ptr<ShaderWrapper> shader = sh.second;
        shader->SetUniform("u_MVP.View", controller.Camera->GetViewMatrix());
        shader->SetUniform("u_MVP.Projection", controller.viewport->GetProjectionMatrix());

        if (sh.first == BEZIER_SHADER)
        {
            shader->SetUniform("u_viewportSize", controller.viewport->GetViewportSize());
        }

        //qDebug() << "Camera space (0,0,0) " << controls->Camera->GetViewMatrix() * QVector4D(0, 0, 0, 1);
        //qDebug() << "NDC space (0,0,0) " << controls->viewport->GetProjectionMatrix() * controls->Camera->GetViewMatrix() * QVector4D(0, 0, 0, 1);
    }
}

void Renderer::UpdateShadersStereo(bool isLeft)
{
    for (auto sh: shaders)
    {
        std::shared_ptr<ShaderWrapper> shader = sh.second;
        if (isLeft)
        {
            shader->SetUniform("u_MVP.View", controller.Camera->GetLeftEyeViewMatrix());
            shader->SetUniform("u_MVP.Projection", controller.viewport->GetLeftEyeProjectionMatrix());
        }
        else
        {
            shader->SetUniform("u_MVP.View", controller.Camera->GetRightEyeViewMatrix());
            shader->SetUniform("u_MVP.Projection", controller.viewport->GetRightEyeProjectionMatrix());
        }
    }
}
