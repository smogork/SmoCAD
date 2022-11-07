//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_LENGTH_H
#define SMOCAD_LENGTH_H

#include "Unit.h"

/**
 * Abstracyjna klasa przedstawiajaca odleglosc w przestzreni.
 *
 */
class Length: public Unit
{
private:
    explicit Length(double dist);
    
public:
    Length ();
    
    static Length FromCentimeters(double cm);
    static Length FromMilimeters(double mm);
    static Length FromMeters(double m);

    double GetMilimeters() const;
    double GetCentimeters() const;
    double GetMeters() const;
    double GetSceneUnits() const;
    
    void SetMilimeters(double mm);
    void SetCentimeters(double cm);
    void SetMeters(double m);
};

#endif //SMOCAD_LENGTH_H
