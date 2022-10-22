//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_UNIT_H
#define SMOCAD_UNIT_H

//Base class for all units
class Unit
{
protected:
    double value;
    
    explicit Unit(double val);
public:
    virtual double GetBaseValue() const;
    
    friend Unit operator+ (const Unit& one, const Unit& two);
    friend Unit operator- (const Unit& one, const Unit& two);
};

#endif //SMOCAD_UNIT_H
