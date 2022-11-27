//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_BLOCKPARAMETERS_H
#define SMOCAD_BLOCKPARAMETERS_H

#include "Mathematics/Units/Length.h"

class BlockParameters
{
public:
    Length Height = Length::FromCentimeters(5.0);
    Length WidthX = Length::FromCentimeters(15.0);
    Length WidthY = Length::FromCentimeters(15.0);
    int TextureWidthX = 2048;
    int TextureWidthY = 2048;
    int VertexWidthX = 512;
    int VertexWidthY = 512;
};

#endif //SMOCAD_BLOCKPARAMETERS_H
