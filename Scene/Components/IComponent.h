//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_ICOMPONENT_H
#define SMOCAD_ICOMPONENT_H


#include <QObject>

class IComponent: public QObject
{
    Q_OBJECT
private:
    unsigned int attachedObjectId;
    unsigned int attachedSystemId;

protected:
    explicit IComponent(unsigned int oid, unsigned sid);
    virtual ~IComponent();
public:
    unsigned int GetAttachedObjectID();
    unsigned int GetAttachedSystemID();
};


#endif //SMOCAD_ICOMPONENT_H
