//
// Created by ksm on 4/22/22.
//

#include "Interpolation.h"
#include "clapack.h"
#include <string>
#include <cmath>

std::vector<QVector3D> Interpolation::C2Interpolation(std::vector<QVector3D> &knots)
{
    std::vector<QVector3D> res;

    if (knots.size() < 2)
        return res;

    res.resize(3 * (knots.size() - 1) + 1);

    std::vector<double> distance(knots.size() - 1);//dystanse przydadza sie poza ta funkcją
    for (int i = 1; i < knots.size(); ++i)
        distance[i - 1] = knots[i].distanceToPoint(knots[i - 1]);
    std::vector<QVector3D> c = SolveInterpolationEquation(knots, distance);
    for (QVector3D &R: c)
        R *= 0.5f;

    //obliczenie współczynników
    std::vector<QVector3D> b(knots.size() - 1), d(knots.size() - 1);
    //zerowe wspolczynniki
    if (qFuzzyIsNull(distance[0]))
        b[0] = d[0] = QVector3D();
    else
    {
        b[0] = (knots[1] - knots[0]) / distance[0] - (c[1] * distance[0]) / 3.0f;
        d[0] = c[1] / (3.0f * distance[0]);
    }

    //kolejne wspolczynniki
    for (int i = 1; i < knots.size() - 1; ++i)
    {
        d[i] = qFuzzyIsNull(distance[i]) ? QVector3D() : (c[i + 1] - c[i]) / (3.0f * distance[i]);
        b[i] = qFuzzyIsNull(distance[i-1]) ? b[i - 1] : b[i - 1] + 2.0f * c[i - 1] * distance[i - 1] +
                                                                          3.0f * d[i - 1] * distance[i - 1] *
                                                                          distance[i - 1];
    }

    //Przetworzenie wspołczynników do punktów Beziera
    for (int i = 0; i < knots.size() - 1; ++i)
    {
        res[3 * i] = knots[i];
        res[3 * i + 1] = knots[i] + (b[i] / 3.0f) * distance[i];
        res[3 * i + 2] = knots[i + 1] -
                         ((b[i] + 2.0f * c[i] * distance[i] + 3.0f * d[i] * distance[i] * distance[i]) / 3.0f) *
                         distance[i];
    }
    res[res.size() - 1] = knots[knots.size() - 1];

    return res;
}

std::vector<QVector3D>
Interpolation::SolveInterpolationEquation(std::vector<QVector3D> &knots, std::vector<double> &distance)
{
    //Przygotuj rozkład LU otrzymanej macierzy trójdiagonalnej
    integer N = knots.size() - 2;
    std::vector<doublereal> diagonal(N, 2);
    std::vector<doublereal> upper2Diagonal(N - 2, 0);
    std::vector<doublereal> upperDiagonal(N - 1), lowerDiagonal(N - 1);
    std::vector<integer> interchanges(N);
    integer info = 0;

    //obliczenie alfa_i, i = 2,...N-2
    for (int i = 0; i < N - 1; i++)
    {
        const int alfa_pivot = 2;
        lowerDiagonal[i] = distance[i + alfa_pivot] / (distance[i + alfa_pivot] + distance[i + alfa_pivot - 1]);
    }
    //obliczanie beta_i i = 1,...,N-3
    for (int i = 1; i < N - 1; ++i)
    {
        //[TODO] to mozna przeorbic aby korzystalo z wyliczonych alf oprocz pierwszego elementu
        const int beta_pivot = 1;
        upperDiagonal[i] = distance[i + beta_pivot] / (distance[i + beta_pivot] + distance[i + beta_pivot - 1]);
    }
    //https://scicomp.stackexchange.com/questions/26395/how-to-start-using-lapack-in-c
    //jak to powinno byc uzywane, a mamy clapack.h sciagniete z lapacka++ http://lapackpp.sourceforge.net/
    dgttrf_(&N, lowerDiagonal.data(), diagonal.data(), upperDiagonal.data(), upper2Diagonal.data(),
            interchanges.data(), &info);

    if (info != 0)
        throw std::invalid_argument("LU sie popsulo. Info:" + std::to_string(info));

    //Przygotowanie macierzy o 3 kolumnach - kazda kolumna to jeden wymiar
    std::vector<QVector3D> res(knots.size());
    std::vector<doublereal> B(3 * N);

    for (int i = 0; i < N; ++i)
    {
        B[i] = 3.0f * (((knots[i + 2].x() - knots[i + 1].x()) / distance[i + 1]) -
                       ((knots[i + 1].x() - knots[i].x()) / distance[i]))
               / (distance[i + 1] + distance[i]);
        B[N + i] = 3.0f * (((knots[i + 2].y() - knots[i + 1].y()) / distance[i + 1]) -
                           ((knots[i + 1].y() - knots[i].y()) / distance[i]))
                   / (distance[i + 1] + distance[i]);
        B[2 * N + i] = 3.0f * (((knots[i + 2].z() - knots[i + 1].z()) / distance[i + 1]) -
                               ((knots[i + 1].z() - knots[i].z()) / distance[i]))
                       / (distance[i + 1] + distance[i]);
    }

    //Rozwaiązanie układu równań dla 3 wymiarów jednocześnie
    integer NHRS = 3;
    char type = 'N';
    dgttrs_(&type, &N, &NHRS, lowerDiagonal.data(), diagonal.data(), upperDiagonal.data(), upper2Diagonal.data(),
            interchanges.data(), B.data(), &N, &info);

    if (info != 0)
        throw std::invalid_argument("Układ równań sie popsul. Info:" + std::to_string(info));

    //Przetworzenie wyniku do QVector3D
    for (int i = 0; i < N; ++i)
        res[i + 1] = QVector3D(B[i], B[N + i], B[2 * N + i]);
    //Warunki interpolacji naturalnej
    res[0] = QVector3D();//0
    res[N + 1] = QVector3D();//0

    return res;
}
