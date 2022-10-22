//
// Created by ksm on 10/20/22.
//

#include "Velocity.h"

Velocity::Velocity(const Distance& dist, const Time& time): Unit(dist.GetMeters() / time.GetBaseValue())
{ }
