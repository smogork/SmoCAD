//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_ICOMPONENT_H
#define SMOCAD_ICOMPONENT_H


#include <QObject>
#include <memory>

class IComponent: public QObject
{
    Q_OBJECT
private:
    unsigned int attachedObjectId;
    unsigned int attachedSystemId;

protected:
    explicit IComponent(unsigned int oid, unsigned sid);

public:
    virtual ~IComponent();
    bool Enabled = true;

    unsigned int GetAttachedObjectID();
    unsigned int GetAttachedSystemID();
};


#endif //SMOCAD_ICOMPONENT_H
