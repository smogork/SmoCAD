//
// Created by ksm on 6/18/22.
//

#include "Optimization.h"
#include <cmath>

QVector4D Optimization::InverseGradientMethod(QVector4D startPoint, std::function<float(QVector4D)> function,
                                              std::function<QVector4D(QVector4D)> gradient, float alpha, float eps,
                                              int max_iter)
{
    QVector4D lastPoint, curPoint = startPoint;
    float lastValue, curValue = function(startPoint);
    QVector4D lastGrad;
    
    int iter_count = 0;
    do
    {
        //przepisanie aktualnej wartosci jako starej
        lastPoint = curPoint;
        lastValue = curValue;
        
        //Obliczenie nowego przyblizenia
        lastGrad = gradient(lastPoint);
        curPoint = lastPoint - alpha * lastGrad;
        curValue = function(curPoint);
        
        //Pomniejszenie aplhy, jeżeli przeskoczyliśmy minimum
        if (curValue >= lastValue)
            alpha /= 2;
        
        iter_count++;
    } while (lastGrad.length() > eps && (lastPoint - curPoint).length() > eps && iter_count < max_iter);
    
    if (iter_count >= max_iter)
        return {NAN, NAN, NAN, NAN};
    return curPoint;
}

QVector4D Optimization::NewtonRaphsonMethod(QVector4D startPoint, std::function<QVector4D(QVector4D)> function,
                                            std::function<QMatrix4x4(QVector4D)> derivative)
{
    return QVector4D();
}
