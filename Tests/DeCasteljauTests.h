//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_DECASTELJAUTESTS_H
#define SMOCAD_DECASTELJAUTESTS_H

#include <gtest/gtest.h>
#include <QVector3D>

class DeCasteljauTests: public testing::Test
{
    const float feps = 1e-6;

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
    
    bool QVector3DAlmostEquals(QVector3D one, QVector3D two);
};

#endif //SMOCAD_DECASTELJAUTESTS_H
