//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_DECASTELJAUTESTS_H
#define SMOCAD_DECASTELJAUTESTS_H

#include <QVector3D>
#include "CommonTestClass.h"

class DeCasteljauTests: public CommonTestClass
{
public:
    DeCasteljauTests( ) {
        // initialization;
        // can also be done in SetUp()
    }
    
    void SetUp( ) {
        // initialization or some code to run before each test
    }
    
    void TearDown( ) {
        // code to run after each test;
        // can be used instead of a destructor,
        // but exceptions can be handled in this function only
    }
    
    ~DeCasteljauTests( ) override {
        //resources cleanup, no exceptions allowed
    }
};

#endif //SMOCAD_DECASTELJAUTESTS_H
