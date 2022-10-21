//
// Created by ksm on 10/20/22.
//

#include "Distance.h"

Distance::Distance(double dist): distance(dist)
{ }

Distance Distance::FromCentimeters(double cm)
{
    Distance res(0);
    res.SetCentimeters(cm);
    return res;
}

Distance Distance::FromMilimeters(double mm)
{
    Distance res(0);
    res.SetMilimeters(mm);
    return res;
}

Distance Distance::FromMeters(double m)
{
    Distance res(0);
    res.SetMeters(mm);
    return res;
}

double Distance::GetBaseValue()
{
    return value;
}

double Distance::GetMilimeters()
{
    return value * 10;
}

double Distance::GetCentimeters()
{
    return value;
}

double Distance::GetMeters()
{
    return value / 100;
}

void Distance::SetMilimeters(double mm)
{
    distance = mm / 10;
}

void Distance::SetCentimeters(double cm)
{
    distance = cm;
}

void Distance::SetMeters(double m)
{
distance = m * 100;
}


