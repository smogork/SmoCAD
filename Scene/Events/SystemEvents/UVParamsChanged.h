//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_UVPARAMSCHANGED_H
#define SMOCAD_UVPARAMSCHANGED_H


#include <QEvent>

class UVParamsChanged: QEvent
{
public:
    float U, V;
    int UDensity, VDensity;

    UVParamsChanged(float u, float v, int ud, int vd): QEvent(User), U(u), V(v), UDensity(ud), VDensity(vd)
    { }
};


#endif //SMOCAD_UVPARAMSCHANGED_H
