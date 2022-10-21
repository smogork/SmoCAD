//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_CUTTERPATH_H
#define SMOCAD_CUTTERPATH_H

#include <vector>
#include <QVector3D>
#include "CutterParameters.h"

class CutterPath
{
public:
    std::vector<QVector3D> Points;
    CutterParameters Cutter;
    
    explicit CutterPath(CutterParameters params);
    CutterPath(const CutterPath& other) = delete;
    CutterPath(CutterPath&& other)  noexcept;
    
    CutterPath& operator=(const CutterPath& right) = delete;
    CutterPath& operator=(CutterPath&& right) noexcept;
};

#endif //SMOCAD_CUTTERPATH_H
