//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_BEZIERC0_H
#define SMOCAD_BEZIERC0_H


#include "VirtualBezierC0.h"
#include "Scene/Components/SceneElement.h"

class BezierC0: public VirtualBezierC0
{
private:
    QPropertyNotifier selectedNotifier;

public:
    std::shared_ptr<Selectable> p_Selected;
    std::shared_ptr<SceneElement> p_SceneElement;

    explicit BezierC0(const QString& name);
};


#endif //SMOCAD_BEZIERC0_H
