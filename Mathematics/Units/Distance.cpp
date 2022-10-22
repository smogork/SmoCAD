//
// Created by ksm on 10/20/22.
//

#include "Distance.h"

Distance::Distance(double dist): Unit(dist)
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
    res.SetMeters(m);
    return res;
}

double Distance::GetMilimeters() const
{
    return value * 10;
}

double Distance::GetCentimeters() const
{
    return value;
}

double Distance::GetMeters() const
{
    return value / 100;
}

void Distance::SetMilimeters(double mm)
{
    value = mm / 10;
}

void Distance::SetCentimeters(double cm)
{
    value = cm;
}

void Distance::SetMeters(double m)
{
    value = m * 100;
}

double Distance::GetSceneUnits() const
{
    return GetCentimeters();
}


