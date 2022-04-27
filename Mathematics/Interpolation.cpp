//
// Created by ksm on 4/22/22.
//

#include "Interpolation.h"
#include "clapack.h"
#include <string>
#include <cmath>

std::vector<QVector3D> Interpolation::C2Interpolation(std::vector<QVector3D> &knots)
{
    if (knots.size() < 2)
        return {};

    if (knots.size() == 2)
    {
        double dist = knots[0].distanceToPoint(knots[1]);
        QVector3D thirdLine = (knots[1] - knots[0]) / 3;
        return { //prosta o rownoodległych wezlach
                knots[0],
                knots[0] + thirdLine,
                knots[0] + 2.0f * thirdLine,
                knots[1]
        };
    }

    std::vector<QVector3D> res(3 * (knots.size() - 1) + 1);

    std::vector<double> distance(knots.size() - 1);//dystanse przydadza sie poza ta funkcją
    for (int i = 1; i < knots.size(); ++i)
        distance[i - 1] = knots[i].distanceToPoint(knots[i - 1]);
    std::vector<QVector3D> c = SolveInterpolationEquation(knots, distance);

    //obliczenie współczynników
    std::vector<QVector3D> b(knots.size() - 1);
    //zerowy współczynnik
    if (qFuzzyIsNull(distance[0]))
        b[0] = QVector3D();
    else
        b[0] = (knots[1] - knots[0]) / distance[0] - distance[0] * c[1] / 3.0f;

    //kolejne wspolczynniki
    for (int i = 1; i < b.size(); ++i)
    {
        if (qFuzzyIsNull(distance[i]))
            b[i] = QVector3D();
        else
            b[i] = (knots[i + 1] - knots[i]) / distance[i] - distance[i] * (c[i + 1] + 2 * c[i]) / 3.0f;
    }

    //ostatni współczynnik
    //int n = b.size() - 1;
    //b[n] = b[n - 1] + (c[n] + c[n - 1]) * distance[n - 1];

    //Przetworzenie wspołczynników do punktów Beziera
    for (int i = 0; i < knots.size() - 2; ++i)
    {
        res[3 * i] = knots[i];
        res[3 * i + 1] = knots[i] + distance[i] * b[i] / 3.0f;
        res[3 * i + 2] = knots[i + 1] - distance[i] * b[i + 1] / 3.0f;
    }

    //ostatni fragment nalezy rozważyc oddzielnie
    int n = b.size() - 1;
    res[3 * n] = knots[n];
    res[3 * n + 1] = knots[n] + distance[n] * b[n] / 3.0f;
    res[3 * n + 2] =
            knots[n + 1] - distance[n] * (b[n] + c[n] * distance[n]) / 3.0f;

    //ostatni węzeł
    res[res.size() - 1] = knots[knots.size() - 1];

    return res;
}

std::vector<QVector3D>
Interpolation::SolveInterpolationEquation(std::vector<QVector3D> &knots, std::vector<double> &distance)
{
    int N = knots.size() - 2;


    /*if (N == 1)
    {
        if (qFuzzyIsNull(distance[0]) || qFuzzyIsNull(distance[1]))
            return {{},{},{}};
        return {
                {0, 0, 0},
                1.5f * ((knots[2] - knots[1]) / distance[1]) -
                ((knots[1] - knots[0]) / distance[0])
                / (distance[0] + distance[1]),
                {0, 0, 0}
        };
    }*/

    std::vector<QVector3D> res(knots.size());
    //Warunki interpolacji naturalnej
    res[0] = QVector3D();//0
    res[N + 1] = QVector3D();//0

    std::vector<float> diagonal(N, 2);
    std::vector<float> upperDiagonal(N), lowerDiagonal(N);

    //obliczenie alfa_i, i = 2,...N-2
    for (int i = 0; i < N; i++)
    {
        if (qFuzzyIsNull(distance[i]) && qFuzzyIsNull(distance[i + 1]))
            lowerDiagonal[i] = 0;
        else
            lowerDiagonal[i] = distance[i] / (distance[i] + distance[i + 1 ]);
    }
    //obliczanie beta_i i = 1,...,N-3
    for (int i = 0; i < N; ++i)
    {
        if (qFuzzyIsNull(distance[i]) && qFuzzyIsNull(distance[i + 1]))
            upperDiagonal[i] = 0;
        else
            upperDiagonal[i] = distance[i + 1] / (distance[i] + distance[i + 1]);
    }

    std::vector<QVector3D> R(N);

    for (int i = 0; i < R.size(); ++i)
    {
        if (qFuzzyIsNull(distance[i + 1]) || qFuzzyIsNull(distance[i]))
            R[i] = QVector3D();
        else
        {
            R[i] = 3.0f * (((knots[i + 2] - knots[i + 1]) / distance[i + 1]) -
                           ((knots[i + 1] - knots[i]) / distance[i]))
                   / (distance[i + 1] + distance[i]);
        }
    }

    for (int i = 1; i < N; ++i)
    {
        float w = lowerDiagonal[i] / diagonal[i-1];
        diagonal[i] -= w * upperDiagonal[i - 1];
        R[i] -= w * R[i-1];
    }
    res[res.size() - 2] = R[N - 1] / diagonal[N - 1];
    for (int i = N - 1; i >= 0; --i)
        res[i + 1] = (R[i] - upperDiagonal[i] * res[i + 2]) / diagonal[i];
    return res;


    /*//Przygotuj rozkład LU otrzymanej macierzy trójdiagonalnej
    integer N = knots.size() - 2;

    if (N == 1)
    {
        if (qFuzzyIsNull(distance[0]) || qFuzzyIsNull(distance[1]))
            return {{},{},{}};
        return {
                {0, 0, 0},
                1.5f * ((knots[2] - knots[1]) / distance[1]) -
                ((knots[1] - knots[0]) / distance[0])
                / (distance[0] + distance[1]),
                {0, 0, 0}
        };
    }

    std::vector<doublereal> diagonal(N, 2);
    std::vector<doublereal> upper2Diagonal(N - 2, 0);
    std::vector<doublereal> upperDiagonal(N - 1), lowerDiagonal(N - 1);
    std::vector<integer> interchanges(N);
    integer info = 0;

    //obliczenie alfa_i, i = 2,...N-2
    for (int i = 0; i < N - 1; i++)
    {
        const int alfa_pivot = 2;
        if (qFuzzyIsNull(distance[i + alfa_pivot]) && qFuzzyIsNull(distance[i + alfa_pivot - 1]))
            lowerDiagonal[i] = 0;
        else
            lowerDiagonal[i] = distance[i + alfa_pivot] / (distance[i + alfa_pivot] + distance[i + alfa_pivot - 1]);
    }
    //obliczanie beta_i i = 1,...,N-3
    for (int i = 1; i < N - 1; ++i)
    {
        //[TODO] to mozna przeorbic aby korzystalo z wyliczonych alf oprocz pierwszego elementu
        const int beta_pivot = 1;
        if (qFuzzyIsNull(distance[i + beta_pivot]) && qFuzzyIsNull(distance[i + beta_pivot - 1]))
            upperDiagonal[i] = 0;
        else
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
        if (qFuzzyIsNull(distance[i + 1]) || qFuzzyIsNull(distance[i]))
            B[i] = B[N + i] = B[2 * N + i] = 0;
            //B[3 * i] = B[3 * i + 1] = B[3 * i + 2] = 0;
        else
        {
            B[i] = 3.0f * (((knots[i + 2].x() - knots[i + 1].x()) / distance[i + 1]) -
                           //B[3 * i] = 3.0f * (((knots[i + 2].x() - knots[i + 1].x()) / distance[i + 1]) -
                           ((knots[i + 1].x() - knots[i].x()) / distance[i]))
                   / (distance[i + 1] + distance[i]);
            B[N + i] = 3.0f * (((knots[i + 2].y() - knots[i + 1].y()) / distance[i + 1]) -
                               //B[3 * i + 1] = 3.0f * (((knots[i + 2].y() - knots[i + 1].y()) / distance[i + 1]) -
                               ((knots[i + 1].y() - knots[i].y()) / distance[i]))
                       / (distance[i + 1] + distance[i]);
            B[2 * N + i] = 3.0f * (((knots[i + 2].z() - knots[i + 1].z()) / distance[i + 1]) -
                                   //B[3 * i + 2] = 3.0f * (((knots[i + 2].z() - knots[i + 1].z()) / distance[i + 1]) -
                                   ((knots[i + 1].z() - knots[i].z()) / distance[i]))
                           / (distance[i + 1] + distance[i]);
        }
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
        //res[i + 1] = QVector3D(B[(int)interchanges[i]], B[N + (int)interchanges[i]], B[2 * N + (int)interchanges[i]]);
        res[i + 1] = QVector3D(B[i], B[N + i], B[2 * N + i]);
        //res[i + 1] = QVector3D(B[3 * i], B[3 * i + 1], B[3 * i + 2]);


    return res;*/
}
