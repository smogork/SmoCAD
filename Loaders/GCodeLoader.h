//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_GCODELOADER_H
#define SMOCAD_GCODELOADER_H

#include <QString>
#include "Scene/Entities/Simulator/CutterPath.h"

class GCodeLoader
{
public:
    static CutterPath LoadCutterPath(const QString& filepath);
    
private:
    static CutterParameters ParseFilenameForParameters(const QString& filepath);
    static QVector3D ParseGCodeLine(const QString& line);
};

#endif //SMOCAD_GCODELOADER_H
