//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_CUTTERPARAMETERS_H
#define SMOCAD_CUTTERPARAMETERS_H

#include "Mathematics/Units/Length.h"

enum CutterType
{
    Spherical,
    Cylindrical
};

class CutterParameters
{
public:
    Length Diameter = Length::FromMilimeters(10);
    CutterType Type = Spherical;
    
    CutterParameters(Length diameter, CutterType type);
    CutterParameters();
};

#endif //SMOCAD_CUTTERPARAMETERS_H
