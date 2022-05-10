//
// Created by ksm on 4/20/22.
//

#ifndef SMOCAD_OPTIONS_H
#define SMOCAD_OPTIONS_H

#include <QProperty>
#include "Renderer/Camera/StereoscopicParameters.h"

class Options
{
public:
    static QProperty<bool> DrawBezierPolygon;
    static QProperty<bool> DrawDeBoorPolygon;
    static QProperty<bool> RenderStereoscopy;
    static std::unique_ptr<StereoscopicParameters> StereoParams;
};


#endif //SMOCAD_OPTIONS_H
