//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_DISTANCE_H
#define SMOCAD_DISTANCE_H

#include "Unit.h"

/**
 * Dystans wyrazony w cm
 *
 */
class Distance: public Unit
{
private:
    explicit Distance(double dist);
    
public:
    static Distance FromCentimeters(double cm);
    static Distance FromMilimeters(double mm);
    static Distance FromMeters(double m);

    double GetMilimeters() const;
    double GetCentimeters() const;
    double GetMeters() const;
    
    void SetMilimeters(double mm);
    void SetCentimeters(double cm);
    void SetMeters(double m);
};

#endif //SMOCAD_DISTANCE_H
