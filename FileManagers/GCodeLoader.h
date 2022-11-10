//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_GCODELOADER_H
#define SMOCAD_GCODELOADER_H

#include <QString>
#include "CutterPath.h"

class GCodeLoader
{
public:
    static std::unique_ptr<CutterPath> LoadCutterPath(const QString& filepath);
    
private:
    static CutterParameters ParseFilenameForParameters(const QString& filepath);
    static QVector3D ParseGCodeLine(const QString& line, QVector3D lastData);
};

#endif //SMOCAD_GCODELOADER_H
