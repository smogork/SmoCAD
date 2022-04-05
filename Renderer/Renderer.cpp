//
// Created by ksm on 4/5/22.
//

#include "Renderer.h"
void Renderer::DrawTriangles(QOpenGLFunctions* functions, unsigned int count)
{
    functions->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawLines(QOpenGLFunctions* functions, unsigned int count)
{
    functions->glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPatches(QOpenGLFunctions* functions, unsigned int count)
{
    functions->glDrawElements(GL_PATCHES, count, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawLineStrip(QOpenGLFunctions* functions, unsigned int count)
{
    functions->glDrawArrays(GL_LINE_STRIP, 0, count);
}

void Renderer::DrawPoints(QOpenGLFunctions *functions, unsigned int count)
{
    functions->glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0);
}
