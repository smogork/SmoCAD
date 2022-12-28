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
    {
        QString line = GenerateGCodeLine(path.Points[i], path.Points[i - 1], lineNumber);
        if (!line.isEmpty())
        {
            out << line << '\n';
            lineNumber++;
        }
    }

    Length totalLength = path.GetLengthOfPath();
    qDebug() << "Routes total length:" << totalLength.GetCentimeters() <<"[cm]. Saving to " <<  filepath;
}

QString GCodeSaver::GenerateFileName(CutterParameters params, int number)
{
    QString res = QString("%0.").arg(number);
    res.append(params.Type == Cylindrical ? "f" : "k");
    //res.append(QString::number((int)std::round(params.Diameter.GetMilimeters()), 'g', 2));
    res.append(QStringLiteral("%1").arg((int)std::round(params.Diameter.GetMilimeters()), 2, 10, QLatin1Char('0')));
    return res;
}

QString GCodeSaver::GenerateGCodeLine(QVector3D point, QVector3D lastPoint, int lineNumber)
{
    QVector3D gpoint = FromSceneToGCode(point);
    QVector3D gLastPoint = FromSceneToGCode(lastPoint);

    if (AlmostEquals(gpoint, gLastPoint, 3))
        return "";

    QString res = QString("N%0G01").arg(lineNumber);

    if (!AlmostEquals(gpoint.x(), gLastPoint.x(), 3))
        res += "X" + QString::number(gpoint.x(), 'f', 3);
    if (!AlmostEquals(gpoint.y(), gLastPoint.y(), 3))
        res += "Y" + QString::number(gpoint.y(), 'f', 3);
    if (!AlmostEquals(gpoint.z(), gLastPoint.z(), 3))
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

bool GCodeSaver::AlmostEquals(float a, float b, int decimalNumbers)
{
    return QString::number(a, 'f', decimalNumbers) == QString::number(b, 'f', decimalNumbers);
}

bool GCodeSaver::AlmostEquals(QVector3D v, QVector3D w, int decimalNumbers)
{
    return AlmostEquals(v.x(), w.x(), decimalNumbers) && AlmostEquals(v.y(), w.y(), decimalNumbers) && AlmostEquals(v.z(), w.z(), decimalNumbers);
}
