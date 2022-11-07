//
// Created by ksm on 5/30/22.
//

#ifndef SMOCAD_POINTSHAPES_H
#define SMOCAD_POINTSHAPES_H

#include <QVector3D>

enum Plane
{
    XY,
    XZ,
    YZ
};

class PointShapes
{
private:
    static void ApplyPlaneTransform(std::vector<QVector3D> &points, QVector3D startPos, Plane plane);
    
    static int GetRectOffset(int wPoints, int hPoints);
    static int GetSingleRectOffset();
    static int GetTubeOffset(int rPoints, int lPoints);
    static int GetHalfSphereOffset(int fiPoints, int thetaPoints);
    static int GetCircleOffset(int fiPoints);
    static void HandleOffset(std::vector<int>& indices, int* offset, int increment);
    
public:
    static std::vector<QVector3D> CreateSingleRect(QVector3D centerPos, float width, float height, Plane plane = XZ);
    static std::vector<QVector3D> CreateRect(QVector3D startPos, float width, float height, int wPoints, int hPoints, Plane plane = XZ);
    static std::vector<QVector3D> CreateTube(QVector3D startPos, float radius, float length, int rPoints, int lPoints, Plane plane = XZ);
    static std::vector<QVector3D> CreateHalfSphere(QVector3D centerPos, float radius, int fiPoints, int thetaPoints, Plane plane = XZ);
    static std::vector<QVector3D> CreateCircle(QVector3D centerPos, float radius, int fiPoints, Plane plane = XZ);
    
    static std::vector<int> RectTriangleIndices(int wPoints, int hPoints, bool clockwise = true, int* offset = nullptr);
    static std::vector<int> TubeTriangleIndices(int rPoints, int lPoints, bool clockwise = true, int* offset = nullptr);
    static std::vector<int> HalfSphereTriangleIndices(int fiPoints, int thetaPoints, bool clockwise = true, int* offset = nullptr);
    static std::vector<int> CircleTriangleIndices(int fiPoints, bool clockwise = true, int* offset = nullptr);

    
    static std::vector<QVector3D> CreateFillPlanePoints(std::vector<QVector3D> edgePoints);
};

#endif //SMOCAD_POINTSHAPES_H
