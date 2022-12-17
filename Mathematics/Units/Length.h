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
    static Length FromSceneUnits(double m);

    double GetMilimeters() const;
    double GetCentimeters() const;
    double GetMeters() const;
    double GetSceneUnits() const;
    
    void SetMilimeters(double mm);
    void SetCentimeters(double cm);
    void SetMeters(double m);

    friend Length operator+ (const Length& one, const Length& two);
    friend Length operator- (const Length& one, const Length& two);
    Length& operator=(const Length& other);
};

#endif //SMOCAD_LENGTH_H
