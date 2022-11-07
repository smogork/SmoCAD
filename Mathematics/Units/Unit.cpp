//
// Created by ksm on 10/20/22.
//

#include "Unit.h"


Unit::Unit(double val): value(val)
{

}

double Unit::GetBaseValue() const
{
    return value;
}

Unit operator+(const Unit &one, const Unit &two)
{
    return Unit(one.value + two.value);
}

Unit operator-(const Unit &one, const Unit &two)
{
    return Unit(one.value - two.value);
}

