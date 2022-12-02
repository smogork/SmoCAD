//
// Created by ksm on 11/10/22.
//

#include <QFile>
#include <QTextStream>
#include <QDir>
#include "GCodeSaver.h"

void GCodeSaver::SaveCutterPath(const QString &folderpath, const CutterPath &path, int number)
{
    if (!QDir(folderpath).exists())
        QDir().mkdir(folderpath);

    QString filepath = folderpath + "/" + GenerateFileName(path.Cutter, number);
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::runtime_error("Cannot open file");

    QTextStream out(&file);
    int lineNumber = 1;
    out << GenerateGCodeLine(path.Points[0], path.Points[0] + QVector3D(1, 1, 1), lineNumber++) << '\n';

    for (int i = 1; i < path.Points.size(); ++i)
        out << GenerateGCodeLine(path.Points[i], path.Points[i - 1], lineNumber++) << '\n';
    qDebug() << "Saving routes file " <<  filepath;
}

QString GCodeSaver::GenerateFileName(CutterParameters params, int number)
{
    QString res = QString("%0.").arg(number);
    res.append(params.Type == Cylindrical ? "f" : "k");
    res.append(QString::number((int)std::round(params.Diameter.GetMilimeters())));
    return res;
}

QString GCodeSaver::GenerateGCodeLine(QVector3D point, QVector3D lastPoint, int lineNumber)
{
    QVector3D gpoint = FromSceneToGCode(point);
    QVector3D gLastPoint = FromSceneToGCode(lastPoint);

    QString res = QString("N%0G01").arg(lineNumber);

    if (gpoint.x() != gLastPoint.x())
        res += "X" + QString::number(gpoint.x(), 'f', 3);
    if (gpoint.y() != gLastPoint.y())
        res += "Y" + QString::number(gpoint.y(), 'f', 3);
    if (gpoint.z() != gLastPoint.z())
        res += "Z" + QString::number(gpoint.z(), 'f', 3);

    return res;
}

QVector3D GCodeSaver::FromSceneToGCode(QVector3D scenePoint)
{
    Length x = Length::FromSceneUnits(scenePoint.x());
    Length y = Length::FromSceneUnits(scenePoint.y());
    Length z = Length::FromSceneUnits(scenePoint.z());
    return {(float)x.GetMilimeters(), (float)-z.GetMilimeters(), (float)y.GetMilimeters()};
}
