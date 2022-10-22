//
// Created by ksm on 5/30/22.
//

#include "PointShapes.h"
#include "Polynomials.h"
#include "Scene/Components/Transform.h"

#include <QMatrix4x4>

std::vector<QVector3D>
PointShapes::CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints, Plane plane)
{
    std::vector<QVector3D> res(wPoints * hPoints);
    
    for (int h = 0; h < hPoints; ++h)//height
        for (int w = 0; w < wPoints; ++w)//width
            res[h * wPoints + w] =
                    QVector3D((float) w * width / (wPoints - 1), 0, (float) h * height / (hPoints - 1))/* + startPos*/;
    
    ApplyPlaneTransform(res, startPos, plane);
    return res;
}

std::vector<QVector3D>
PointShapes::CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints, Plane plane)
{
    std::vector<QVector3D> res(rPoints * lPoints);
    
    for (int i = 0; i < lPoints; ++i)//height
        for (int j = 0; j < rPoints; ++j)//width
            res[i * rPoints + j] = QVector3D(
                    radius * cos(2 * M_PI * j / rPoints),
                    radius * sin(2 * M_PI * j / rPoints),
                    (float) i * length / (lPoints - 1));
    
    ApplyPlaneTransform(res, startPos, plane);
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
            firstInnerPoints.push_back(edgeDoubles[i].first[j] + (edgeDoubles[i].first[j] - deeperDoubles[i].first[j]));
            secondInnerPoints
                    .push_back(edgeDoubles[i].second[j] + (edgeDoubles[i].second[j] - deeperDoubles[i].second[j]));
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

std::vector<QVector3D> PointShapes::CreateSingleRect(QVector3D centerPos, float width, float height, Plane plane)
{
    std::vector<QVector3D> res = {
            {-0.5f * width, 0.0f, 0.5f * height},
            {0.5f * width,  0.0f, 0.5f * height},
            {-0.5f * width, 0.0f, -0.5f * height},
            {0.5f * width,  0.0f, -0.5f * height},
    };
    
    ApplyPlaneTransform(res, centerPos, plane);
    return res;
}

void PointShapes::ApplyPlaneTransform(std::vector<QVector3D> &points, QVector3D startPos, Plane plane)
{
    QMatrix4x4 transform;
    
    transform.translate(startPos);
    switch (plane)
    {
        case XY:
            transform.rotate(-90, Transform::GetXAxis());
            break;
        case XZ:
            break;
        case YZ:
            transform.rotate(-90, Transform::GetZAxis());
            transform.rotate(-90, Transform::GetYAxis());
            break;
    }
    
    for (QVector3D &p: points)
    {
        QVector4D p4 = p.toVector4D();
        p4.setW(1.0f);
        p = (transform * p4).toVector3D();
    }
}

std::vector<int> PointShapes::RectTriangleIndices(int wPoints, int hPoints, bool clockwise, int* offset)
{
    std::vector<int> indices;
    indices.reserve((wPoints - 1) * (hPoints - 1) * 6);
    
    for (int h = 0; h < hPoints - 1; ++h)
        for (int w = 0; w < wPoints - 1; ++w)
        {
            indices.push_back(w + h * wPoints);
            if (clockwise)
            {
                indices.push_back(w + (h + 1) * wPoints);
                indices.push_back(w + 1 + (h + 1) * wPoints);
            } else
            {
                indices.push_back(w + 1 + (h + 1) * wPoints);
                indices.push_back(w + (h + 1) * wPoints);
            }
            
            indices.push_back(w + h * wPoints);
            if (clockwise)
            {
                indices.push_back(w + 1 + (h + 1) * wPoints);
                indices.push_back(w + 1 + (h) * wPoints);
            } else
            {
                indices.push_back(w + 1 + (h) * wPoints);
                indices.push_back(w + 1 + (h + 1) * wPoints);
            }
        }
    
    HandleOffset(indices, offset, GetRectOffset(wPoints, hPoints));
    return indices;
}

std::vector<int> PointShapes::TubeTriangleIndices(int rPoints, int lPoints, bool clockwise, int* offset)
{
    //Wszystko poza zlaczeniem jest jak plaszczyzna
    std::vector<int> indices = RectTriangleIndices(rPoints, lPoints, clockwise, nullptr);
    
    //Zlaczenie
    for (int i = 0; i < lPoints - 1; ++i)
    {
        indices.push_back(rPoints * (i + 1) - 1 );
        if (clockwise)
        {
            indices.push_back(rPoints * (i + 2) - 1);
            indices.push_back(rPoints * i);
        }
        else
        {
            indices.push_back(rPoints * i);
            indices.push_back(rPoints * (i + 2) - 1);
        }
    
        indices.push_back(rPoints * (i + 2) - 1 );
        if (clockwise)
        {
            indices.push_back(rPoints * (i + 1));
            indices.push_back(rPoints * i);
        }
        else
        {
            indices.push_back(rPoints * i);
            indices.push_back(rPoints * (i + 1));
        }
    }
    
    HandleOffset(indices, offset, GetTubeOffset(rPoints, lPoints));
    return indices;
}

std::vector<QVector3D>
PointShapes::CreateHalfSphere(QVector3D centerPos, float radius, int fiPoints, int thetaPoints, Plane plane)
{
    std::vector<QVector3D> res;
    
    for (int j = 0; j < thetaPoints; ++j)
    {
        float theta = (float)j / thetaPoints * M_PI * 0.5f;
        for (int i = 0; i < fiPoints; ++i)
        {
            float fi = (float)i / (fiPoints - 1) * 2.0f * M_PI;
            res.push_back(QVector3D(
                    radius * cosf(theta) * cosf(fi),
                    -radius * sinf(theta),
                    radius * cosf(theta) * sinf(fi)
            ));
        }
    }
    res.push_back(QVector3D(0, radius, 0));//czubek sfery
    
    ApplyPlaneTransform(res, centerPos, plane);
    return res;
}

std::vector<int> PointShapes::HalfSphereTriangleIndices(int fiPoints, int thetaPoints, bool clockwise, int* offset)
{
    return std::vector<int>();
}

std::vector<QVector3D> PointShapes::CreateCircle(QVector3D centerPos, float radius, int fiPoints, Plane plane)
{
    std::vector<QVector3D> res;
    
    for (int i = 0; i < fiPoints; ++i)
    {
        float fi = (float)i / (fiPoints - 1)* 2.0f * M_PI;
        res.push_back(QVector3D(
                radius * cosf(fi),
                0,
                radius * sinf(fi)
                ));
    }
    res.push_back(QVector3D(0, 0, 0));//srodek kola
    
    ApplyPlaneTransform(res, centerPos, plane);
    return res;
}

std::vector<int> PointShapes::CircleTriangleIndices(int fiPoints, bool clockwise, int* offset)
{
    int center = fiPoints;
    std::vector<int> indices;
    indices.reserve((fiPoints - 1) * 3);
    
    for (int i = 0; i < fiPoints - 1; ++i)
    {
        indices.push_back(center);
        if (clockwise)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        else
        {
            indices.push_back(i + 1);
            indices.push_back(i);
        }
    }
    
    HandleOffset(indices, offset, GetCircleOffset(fiPoints));
    return indices;
}

int PointShapes::GetRectOffset(int wPoints, int hPoints)
{
    return wPoints * hPoints;
}

int PointShapes::GetTubeOffset(int rPoints, int lPoints)
{
    return GetRectOffset(rPoints, lPoints);
}

int PointShapes::GetSingleRectOffset()
{
    return 4;
}

int PointShapes::GetHalfSphereOffset(int fiPoints, int thetaPoints)
{
    return fiPoints * thetaPoints + 1;
}

int PointShapes::GetCircleOffset(int fiPoints)
{
    return fiPoints + 1;
}

void PointShapes::HandleOffset(std::vector<int>& indices, int* offset, int increment)
{
    if (offset)
    {
        if (*offset > 0)
            for (auto &idx: indices)
                idx += *offset;
        *offset += increment;
    }
}


