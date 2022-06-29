//
// Created by ksm on 5/30/22.
//

#include "PointShapes.h"
#include "Polynomials.h"

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
            res[i * rPoints + j] = QVector3D(radius * cos(2 * M_PI * j / rPoints), radius * sin(2 * M_PI * j / rPoints),
                                             (float) i  * length / (lPoints - 1))+ startPos;

    return res;
}

std::vector<QVector3D> PointShapes::CreateFillPlanePoints(std::vector<QVector3D> edgePoints)
{
    int planeNum = edgePoints.size() / 8;
    if (planeNum < 3)
        throw std::runtime_error("We can only fill hole with at least 3 edges");
    std::vector<QVector3D> res;
    
    std::vector<std::pair<std::vector<QVector3D>, std::vector<QVector3D>>> edgeDoubles, deeperDoubles;
    int size = 4;
    
    //Wyznaczenie funkcji dzielacych krawedzie na pol
    for (int i = 0; i < planeNum; ++i)
    {
        edgeDoubles.push_back(Polynomials::deCasteljauDouble(0.5f, edgePoints.data() + 8 * i, size));
        deeperDoubles.push_back(Polynomials::deCasteljauDouble(0.5f, edgePoints.data() + 8 * i + size, size));
    }
    
    //Wyznaczenie punktow dla platkow przy krawedzi
    std::vector<std::pair<std::vector<QVector3D>, std::vector<QVector3D>>> innerEdgePoints;
    for (int i = 0; i < planeNum; ++i)
    {
        std::vector<QVector3D> firstInnerPoints, secondInnerPoints;
        
        for (int j = 0; j < 4; ++j)
        {
            firstInnerPoints.push_back(edgeDoubles[i].first[j]  + (edgeDoubles[i].first[j]- deeperDoubles[i].first[j]));
            secondInnerPoints.push_back(edgeDoubles[i].second[j] + (edgeDoubles[i].second[j] - deeperDoubles[i].second[j]));
        }
        
        innerEdgePoints.push_back(std::make_pair(firstInnerPoints, secondInnerPoints));
    }
    
    //Wyznaczenie punktow na "krzyzaku" laczacego platki
    
    QVector3D centerPoint;
    std::vector<QVector3D> QPoints, centerInnerPoints;
    for (int i = 0; i < planeNum; ++i)
    {
        QPoints.push_back(edgeDoubles[i].second[0] + 3 * (innerEdgePoints[i].second[0] - edgeDoubles[i].second[0]) / 2);
        centerPoint += QPoints.back();
    }
    centerPoint /= planeNum;
    
    for (int i = 0; i < QPoints.size(); ++i)
        centerInnerPoints.push_back((2 * QPoints[i] + centerPoint) / 3);
    
    //Wyznaczenie nieszczesnych punktow 7 i 8
    std::vector<QVector3D> sevens(planeNum), eights(planeNum);
    for (int i = 0; i < planeNum; ++i)
        sevens[i] = centerInnerPoints[i] + (edgeDoubles[(i + 1) % planeNum].first[2] - centerInnerPoints[i]) / 4.0f;
    
    for (int i = 0; i < planeNum; ++i)
        eights[i] = 2 * centerInnerPoints[(i + 1) % planeNum] - sevens[(i + 1) % planeNum];
    
    //Ulozenie punktow do kolekcji wyjsciowej
    for (int i = 0; i < planeNum; ++i)
    {
        //pierwsza warstwa
        for (int j = 0; j <= 3; ++j)
            res.push_back(edgeDoubles[i].second[j]);
        
        //druga warstwa
        res.push_back(innerEdgePoints[i].second[0]);
        res.push_back(innerEdgePoints[i].second[1]);
        res.push_back(innerEdgePoints[i].second[1]);
        res.push_back(innerEdgePoints[i].second[2]);
        res.push_back(innerEdgePoints[(i + 1) % planeNum].first[1]);
        res.push_back(edgeDoubles[(i + 1) % planeNum].first[1]);
        
        //trzecia warstwa
        res.push_back(centerInnerPoints[i]);
        res.push_back(sevens[i]);
        res.push_back(eights[i]);
        res.push_back(innerEdgePoints[(i + 1) % planeNum].first[2]);
        res.push_back(innerEdgePoints[(i + 1) % planeNum].first[2]);
        res.push_back(edgeDoubles[(i + 1) % planeNum].first[2]);
        
        //czwarta warstwa
        res.push_back(centerPoint);
        res.push_back(centerInnerPoints[(i + 1) % planeNum]);
        res.push_back(innerEdgePoints[(i + 1) % planeNum].first[3]);
        res.push_back(edgeDoubles[(i + 1) % planeNum].first[3]);
    }
    
    return res;
}
