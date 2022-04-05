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
    DEFAULT,
    CURSOR,
    BEZIER,
    SHADERS_COUNT
};

class Renderer
{
private:
    static std::map<int, std::shared_ptr<ShaderWrapper>> shaders;

public:
    static void AddShader(SHADERS id, std::shared_ptr<ShaderWrapper> sh);
    static std::weak_ptr<ShaderWrapper> GetShader(SHADERS id);
    static void UpdateShaders(std::shared_ptr<InputController> controls);
    static void DeleteShaders();

#pragma region Draw primitives
    static void DrawTriangles(QOpenGLFunctions* functions, unsigned int count);
    static void DrawLines(QOpenGLFunctions* functions, unsigned int count);
    static void DrawPatches(QOpenGLFunctions* functions, unsigned int count);
    static void DrawLineStrip(QOpenGLFunctions* functions, unsigned int count);
    static void DrawPoints(QOpenGLFunctions* functions, unsigned int count);
#pragma endregion
};


#endif //SMOCAD_RENDERER_H
