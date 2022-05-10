//
// Created by ksm on 4/3/22.
//

#include "DrawingSystem.h"
#include "Renderer/Options.h"

void DrawingSystem::Render(QOpenGLContext* context)
{
    if (Options::RenderStereoscopy)
        StereoscopyRender(context);
    else
        PlainRender(context);

}

void DrawingSystem::PlainRender(QOpenGLContext* context)
{
    for (const std::pair<unsigned int, std::weak_ptr<Drawing>> &p : components)
        if (auto obj = p.second.lock())
            if (obj->Enabled)
                obj->Render(context);
}

void DrawingSystem::StereoscopyRender(QOpenGLContext* context)
{
    auto gl = context->functions();

    //Skonfiguruj blending

    gl->glColorMask(false, true, true, true);
    //ustaw macierz perspektywy na lewe oko
    PlainRender(context);

    glClear(GL_DEPTH_BUFFER_BIT);

    gl->glColorMask(true, false, false, true);
    //ustaw macierz perspektywy na prawe oko
    PlainRender(context);
}
