//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_INTERPOLATION_H
#define SMOCAD_INTERPOLATION_H

#include <QVector3D>
#include <list>

//Statyczna klasa odpowiadająca za intepolację krzywych
class Interpolation
{
private:
    //zwraca druga pochodna w każdym kierunku
    static std::vector<QVector3D> SolveInterpolationEquation(std::vector<QVector3D>& knots, std::vector<double>& distance);

public:
    //Tworzy krzywą interpolacyjną o ciągłości C2 przedstawioną w postaci krzywej sklejanej w bazie Bernsteina.
    //Wynik będzie zawierać dokładnie 3(n-1) + 1 punktów (wspólne punkty przy sklejeniu będa podane tylko raz)
    //Przy punktach wielokrotnych będzie ciągłośc C0
    static std::vector<QVector3D> C2Interpolation(std::vector<QVector3D>& knots);
};

#endif //SMOCAD_INTERPOLATION_H
