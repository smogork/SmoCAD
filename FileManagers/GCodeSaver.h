//
// Created by ksm on 11/10/22.
//

#ifndef SMOCAD_GCODESAVER_H
#define SMOCAD_GCODESAVER_H

#include <QString>
#include "CutterPath.h"

class GCodeSaver
{
private:

public :
    static void SaveCutterPath(const QString& filepath, const CutterPath& path);
};

#endif //SMOCAD_GCODESAVER_H
