//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_LAZYRENDERER_H
#define SMOCAD_LAZYRENDERER_H


#include <QVector3D>

class [[maybe_unused]] LazyRenderer
{
public:
    static QVector3D GetXAxis() { return {1.0, 0.0, 0.0}; }

    static QVector3D GetYAxis() { return {0.0, 1.0, 0.0}; }

    static QVector3D GetZAxis() { return {0.0, 0.0, 1.0}; }
};


#endif //SMOCAD_LAZYRENDERER_H
