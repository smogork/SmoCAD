//
// Created by ksm on 5/17/22.
//

#ifndef SMOCAD_UVPLANECREATOR_H
#define SMOCAD_UVPLANECREATOR_H

#include "IComponent.h"
#include "Transform.h"
#include "Scene/Entities/Planes/PlaneC0.h"

#define PATCH_SIZE 4

enum Plane
{
    XY,
    XZ,
    YZ
};

class UVPlaneCreator : public IComponent
{
Q_OBJECT

private:
    std::shared_ptr<Transform> m_transform = nullptr;

    std::vector<std::shared_ptr<Point>> CreatePoints(const QString &name, int w, int h, Plane p = XY);

public:
    QProperty<int> U;
    QProperty<int> V;
    QProperty<int> UDensity;
    QProperty<int> VDensity;
    QProperty<double> Width, Height;
    QProperty<bool> IsPipe;

    explicit UVPlaneCreator(unsigned int oid);
    ~UVPlaneCreator() override;

    static std::shared_ptr<UVPlaneCreator>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, int U, int V);
    void UnregisterComponent();

    std::shared_ptr<PlaneC0> CreatePlainC0(const QString &name);
};

#endif //SMOCAD_UVPLANECREATOR_H