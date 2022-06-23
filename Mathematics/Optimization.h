//
// Created by ksm on 6/18/22.
//

#ifndef SMOCAD_OPTIMIZATION_H
#define SMOCAD_OPTIMIZATION_H

#include <QVector4D>
#include <QMatrix4x4>

class Optimization
{
public:
    // Znajdz minimum podanej funkcji R4 -> R, gdu znamy jej gradinet.
    // Zwaraca argument, gdzie osiągane jest minimum lokalne dla zadanego punktu początkowego.
    // Jeżeli w wyniku werktor zawiera NANy, to minimum nie zostało osiągnięte.
    static QVector4D SimpleGradientMethod(QVector4D startPoint, std::function<float(QVector4D args)> function,
                                          std::function<QVector4D(QVector4D args)> gradient, float alpha = 0.05f,
                                          float eps = 1e-4, int max_iter = 1000);
    
    // Znajdx miejsce zerowe funkcji R4 -> R4, gdy znamy jej pochodną.
    // Należy podac funkcję podającą wartość w postaci wektora oraz fukcje podającą pochodna w postaci macierzy.
    // Zwaraca argument, gdzie osiągane jest miejsce zerowe dla zadanego punktu początkowego.
    // Jeżeli w wyniku werktor zawiera NANy, to miejsce zerowe nie zostało osiągnięte.
    static QVector4D NewtonRaphsonMethod(QVector4D startPoint, std::function<QVector4D(QVector4D args)> function,
                                         std::function<QMatrix4x4(QVector4D args)> derivative,
                                         float eps = 1e-4, int max_iter = 1000);
};

#endif //SMOCAD_OPTIMIZATION_H
