//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_IENTITY_H
#define SMOCAD_IENTITY_H

#include <QObject>

enum ENTITY_CLASS
{
    POINT_CLASS,
    GRID_CLASS,
    CUBE_CLASS,
    TORUS_CLASS,
    CURSOR_CLASS,
    COMPOSITE_CLASS,
    COMPOSITE_ELEM_CLASS,
    CLASS_COUNT
};

class IEntity: public QObject
{
    Q_OBJECT
protected:
    unsigned int objectID;
    unsigned int classID;

public:

    explicit IEntity(unsigned int cid);
    virtual ~IEntity() override;

    unsigned int GetObjectID();
    unsigned int GetClassID();
};


#endif //SMOCAD_IENTITY_H
