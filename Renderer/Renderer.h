//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_RENDERER_H
#define SMOCAD_RENDERER_H


#include <QOpenGLFunctions>

class Renderer
{
public:
#pragma region Draw primitives
    static void DrawTriangles(QOpenGLFunctions* functions, unsigned int count);
    static void DrawLines(QOpenGLFunctions* functions, unsigned int count);
    static void DrawPatches(QOpenGLFunctions* functions, unsigned int count);
    static void DrawLineStrip(QOpenGLFunctions* functions, unsigned int count);
    static void DrawPoints(QOpenGLFunctions* functions, unsigned int count);
#pragma endregion
};


#endif //SMOCAD_RENDERER_H
