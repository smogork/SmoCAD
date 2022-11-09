//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_RENDERER_H
#define SMOCAD_RENDERER_H

#include <QOpenGLFunctions>
#include "ShaderWrapper.h"
#include "Renderer/InputController/InputController.h"

enum SHADERS
{
    DEFAULT_SHADER,
    CURSOR_SHADER,
    BEZIER_SHADER,
    BEZIERC2_SHADER,
    PLANE_SHADER,
    PLANEC2_SHADER,
    SELECT_RECT_SHADER,
    FILL_PLANE_SHADER,
    TORUS_SHADER,
    BLOCK_LOWERWALL_SHADER,
    BLOCK_UPPERWALL_SHADER,
    BLOCK_SIDEWALL_SHADER,
    SIMULATOR_CUTTER_SHADER,
    SHADERS_COUNT
};

class Renderer
{
private:
    static std::map<int, std::shared_ptr<ShaderWrapper>> shaders;

public:
    static InputController controller;
    static QProperty<double> EyeSeparation;

    static void AddShader(SHADERS id, std::shared_ptr<ShaderWrapper> sh);
    static std::weak_ptr<ShaderWrapper> GetShader(SHADERS id);
    static void UpdateShaders();
    static void UpdateShadersStereo(bool isLeft);
    static void DeleteShaders();

    template<class T>
    static void SetUniformAtAllShaders(const QString &name, const T &val)
    {
        for (auto sh: shaders)
        {
            if (sh.first == SELECT_RECT_SHADER)
                continue;

            std::shared_ptr<ShaderWrapper> shader = sh.second;
            shader->SetUniform(name.toStdString(), val);
        }
    }

#pragma region Draw primitives
    static void DrawTriangles(QOpenGLFunctions *functions, unsigned int count);
    static void DrawLines(QOpenGLFunctions *functions, unsigned int count);
    static void DrawPatches(QOpenGLFunctions *functions, unsigned int count);
    static void DrawLineStrip(QOpenGLFunctions *functions, unsigned int count);
    static void DrawPoints(QOpenGLFunctions *functions, unsigned int count);
#pragma endregion
};

#endif //SMOCAD_RENDERER_H
