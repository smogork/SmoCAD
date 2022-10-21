//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_CUTTERPARAMETERS_H
#define SMOCAD_CUTTERPARAMETERS_H

enum CutterType
{
    Spherical,
    Cylindrical
};

class CutterParameters
{
public:
    double Diameter;
    CutterType Type;
    int
    
    CutterParameters(double diameter, CutterType type);
};

#endif //SMOCAD_CUTTERPARAMETERS_H
