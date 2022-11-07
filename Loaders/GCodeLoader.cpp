//
// Created by ksm on 10/20/22.
//

#include "GCodeLoader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

std::unique_ptr<CutterPath> GCodeLoader::LoadCutterPath(const QString &filepath)
{
    CutterParameters params = ParseFilenameForParameters(filepath);
    std::unique_ptr<CutterPath> ret = std::make_unique<CutterPath>(params);
    
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("Cannot open file");
    
    QTextStream in(&file);
    QVector3D lastData = {-1, -1, -1};
    while (!in.atEnd())
    {
        try
        {
            QString line = in.readLine();
            QVector3D move = ParseGCodeLine(line, lastData);
            lastData = move;
            ret->Points.push_back(move);
        }
        catch (const std::runtime_error &e)
        {
            qDebug() << "Error while parsing a GCode line: " << e.what();
        }
    }
    
    return ret;
}

CutterParameters GCodeLoader::ParseFilenameForParameters(const QString &filepath)
{
    int lastDot = filepath.lastIndexOf('.');
    if (lastDot + 4 == filepath.size() && (filepath[lastDot + 1] == 'k' || filepath[lastDot + 1] == 'f'))
    {
        CutterParameters res;
        res.Type = filepath[lastDot + 1] == 'k' ? Spherical : Cylindrical;
        res.Diameter = Length::FromMilimeters(std::atoi(filepath.mid(lastDot + 2, 2).toStdString().c_str()));
        return res;
    }
    throw std::runtime_error("Wrong extension of file");
}

QVector3D GCodeLoader::ParseGCodeLine(const QString &line, QVector3D lastData)
{
    std::map<QChar, float> data;
    data['X'] = lastData.x();
    data['Y'] = -lastData.z();
    data['Z'] = lastData.y();
    
    int it = 0;
    //znajdz pierwsza literke
    while (line[it] < 'A' or line[it] > 'Z') it++;
    
    while (it < line.size())
    {
        QChar l = line[it];
        float val;
        int last = it;
    
        it++;
        while (it < line.size() and (line[it] < 'A' or line[it] > 'Z')) it++;
        val = Length::FromMilimeters(std::strtof(line.mid(last + 1, it - last - 1).toStdString().c_str(), nullptr)).GetSceneUnits();
        data[l] = val;
    }
    
    return {data['X'], data['Z'], -data['Y']};
}
