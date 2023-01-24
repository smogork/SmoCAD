//
// Created by ksm on 10/20/22.
//

#include "Length.h"

Length::Length(double dist) : Unit(dist)
{}

Length::Length() : Unit(0)
{

}

Length Length::FromCentimeters(double cm)
{
    Length res(0);
    res.SetCentimeters(cm);
    return res;
}

Length Length::FromMilimeters(double mm)
{
    Length res(0);
    res.SetMilimeters(mm);
    return res;
}

Length Length::FromMeters(double m)
{
    Length res(0);
    res.SetMeters(m);
    return res;
}

Length Length::FromSceneUnits(double su)
{
    return FromCentimeters(su);
}

double Length::GetMilimeters() const
{
    return value * 10;
}

double Length::GetCentimeters() const
{
    return value;
}

double Length::GetMeters() const
{
    return value / 100;
}

void Length::SetMilimeters(double mm)
{
    value = mm / 10;
}

void Length::SetCentimeters(double cm)
{
    value = cm;
}

void Length::SetMeters(double m)
{
    value = m * 100;
}

double Length::GetSceneUnits() const
{
    return GetCentimeters();
}

Length &Length::operator=(const Length& other)
{
    // Guard self assignment
    if (this == &other)
        return *this;

    Unit::operator=(other);
    return *this;
}

Length operator+(const Length &one, const Length &two)
{
    return Length(one.value + two.value);
}

Length operator-(const Length &one, const Length &two)
{
    return Length(one.value - two.value);
}



