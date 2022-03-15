//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_TORUSOBJECT_H
#define SMOCAD_TORUSOBJECT_H


#include "TransformableObject.h"
#include "IRenderableObject.h"

class TorusObject: public TransformableObject, public IRenderableObject
{
private:
    float biggerR;
    float smallerR;
    int biggerRDensity;
    int smallerRDensity;

public:

    TorusObject(QVector3D pos, float R, float r, int RDensity, int rDensity);

    void SetBiggerRadius(float value);
    void SetSmallerRadius(float value);
    void SetBiggerRadiusDensity(int value);
    void SetSmallerRadiusDensity(int value);

    float GetBiggerR();
    float GetSmallerR();
    int GetBiggerRDensity();
    int GetSmallerRDensity();

    std::vector<float> GenerateGeometryVertices() override;
    std::vector<int> GenerateTopologyEdges() override;
    int GetIndexCount() override;

};


#endif //SMOCAD_TORUSOBJECT_H
