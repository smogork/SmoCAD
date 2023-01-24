//
// Created by ksm on 12/2/22.
//

#ifndef SMOCAD_POLYLINEOPERATIONS_H
#define SMOCAD_POLYLINEOPERATIONS_H

#include <QVector2D>
#include <list>



class PolylineOperations
{
public:
    static std::list<QVector2D>** CreatePlaneDivision(const std::vector<std::pair<std::vector<QVector2D>, bool>>& polylines, QVector4D PlaneSize, int divisionSize = 128);
    static std::vector<QVector2D> JoinPolylinesTogether(const std::vector<std::pair<std::vector<QVector2D>, bool>>& polylines, std::list<QVector2D>** planeDivision);
};

#endif //SMOCAD_POLYLINEOPERATIONS_H
