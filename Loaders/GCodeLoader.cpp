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
        throw std::runtime_error(QString("Cannot open file %0").arg(filepath).toStdString());
    
    QTextStream in(&file);
    while (!in.atEnd())
    {
        try
        {
            QString line = in.readLine();
            QVector3D move = ParseGCodeLine(line);
            ret->Points.push_back(move);
        }
        catch (const std::runtime_error &e)
        {
            qDebug() << "Error while parsing a GCode line at file " << filepath << ": " << e.what();
        }
    }
    
    return ret;
}

CutterParameters GCodeLoader::ParseFilenameForParameters(const QString &filepath)
{
    return {1, Cylindrical};
}

QVector3D GCodeLoader::ParseGCodeLine(const QString &line)
{
    QVector3D res = {-1, -1, -1};
    int Gpos = -1, Xpos = -1, Ypos = -1, Zpos = -1;
    int it = 0;
    
    for (; it < line.size() && line[it] != 'G'; ++it);
    Gpos = it;
    for (; it < line.size() && line[it] != 'X'; ++it);
    Xpos = it;
    for (; it < line.size() && line[it] != 'Y'; ++it);
    Ypos = it;
    for (; it < line.size() && line[it] != 'Z'; ++it);
    Zpos = it;
    
    if (Xpos - Gpos != 3 || Xpos * Ypos * Zpos < 0)
        throw std::runtime_error(QString("Wrong line to parse: %0").arg(line).toStdString());
    
    //TODO: Pozostal jeszcze przypadek gdy jakiejs wspolrzednej zabraknie. Wystarczy wtedy -1 wystawic i poprawic w gornej petli.
    res.setX(std::strtof(line.mid(Xpos + 1, Ypos - Xpos - 1).toStdString().c_str(), nullptr));
    res.setZ(std::strtof(line.mid(Ypos + 1, Zpos - Ypos - 1).toStdString().c_str(), nullptr));
    res.setY(std::strtof(line.mid(Zpos + 1, line.size() - Ypos - 1).toStdString().c_str(), nullptr));
    
    return res / 10;//TODO; zastosowac obiekty z przetwarzaniem odleglosci
}
