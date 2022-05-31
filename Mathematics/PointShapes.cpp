//
// Created by ksm on 5/30/22.
//

#include "PointShapes.h"

std::vector<QVector3D> PointShapes::CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints)
{
    std::vector<QVector3D> res(wPoints * hPoints);

    for (int i = 0; i < hPoints; ++i)//height
        for (int j = 0; j < wPoints; ++j)//width
            res[i * wPoints + j] = QVector3D((float) j * width / (wPoints - 1), 0, (float) i * height / (hPoints - 1)) + startPos;

    return res;
}

std::vector<QVector3D> PointShapes::CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints)
{
    std::vector<QVector3D> res(rPoints * lPoints);

    for (int i = 0; i < lPoints; ++i)//height
        for (int j = 0; j < rPoints; ++j)//width
            res[i * rPoints + j] = QVector3D(radius * cos(2 * M_PIf * j / rPoints), radius * sin(2 * M_PIf * j / rPoints),
                                             (float) i  * length / (lPoints - 1));

    return res;
}
