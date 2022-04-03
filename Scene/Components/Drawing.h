//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWING_H
#define SMOCAD_DRAWING_H


#include "IComponent.h"

class Drawing: public IComponent
{
public:

    static std::shared_ptr<Drawing> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();
};


#endif //SMOCAD_DRAWING_H
