//
// Created by ksm on 10/20/22.
//

#include "Velocity.h"

Velocity::Velocity(Distance dist, Time time)
{
    value = dist.GetMeters() / time.GetBaseValue();
}
