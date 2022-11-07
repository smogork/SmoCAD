//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_VELOCITY_H
#define SMOCAD_VELOCITY_H

#include "Unit.h"
#include "Time.h"
#include "Length.h"

class Velocity: public Unit
{
public:
    Velocity(const Length& dist, const Time& time);
};

#endif //SMOCAD_VELOCITY_H
