//
// Created by ksm on 6/19/22.
//

#ifndef SMOCAD_INTERSECTIONAWARE_H
#define SMOCAD_INTERSECTIONAWARE_H

#include "Scene/Components/IComponent.h"
#include "Scene/Components/UVParams.h"

class IntersectionAware: public IComponent
{
public:
    //[TODO] dodac ograniczenia na zakesy parametrow
    std::shared_ptr<UVParams> p_UV;
    
    static std::shared_ptr<IntersectionAware> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv);
    void UnregisterComponent();
    
    explicit IntersectionAware(unsigned int oid);
    ~IntersectionAware() override;
};

#endif //SMOCAD_INTERSECTIONAWARE_H
