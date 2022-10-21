//
// Created by ksm on 10/20/22.
//

#include "Unit.h"

double Unit::GetBaseValue()
{
    return value;
}

Unit &operator+(const Unit &one, const Unit &two)
{
    Unit res;
    res.value = one.value + two.value;
    return res;
}

Unit &operator-(const Unit &one, const Unit &two)
{
    Unit res;
    res.value = one.value - two.value;
    return res;
}
