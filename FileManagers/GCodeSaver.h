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
    static QString GenerateFileName(CutterParameters params, int number);
    static QString GenerateGCodeLine(QVector3D point, QVector3D lastPoint, int lineNumber);
    static QVector3D FromSceneToGCode(QVector3D scenePoint);
public :
    static void SaveCutterPath(const QString& folderpath, const CutterPath& path, int number);
};

#endif //SMOCAD_GCODESAVER_H
