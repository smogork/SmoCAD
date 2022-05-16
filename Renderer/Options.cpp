//
// Created by ksm on 4/20/22.
//

#include "Options.h"

QProperty<bool> Options::DrawBezierPolygon;
QProperty<bool> Options::DrawDeBoorPolygon;
QProperty<bool> Options::DrawPlainMesh;
QProperty<bool> Options::RenderStereoscopy;
std::unique_ptr<StereoscopicParameters> Options::StereoParams = std::make_unique<StereoscopicParameters>();