//
// Created by ksm on 4/13/22.
//

#include "BezierC0.h"

BezierC0::BezierC0(const QString &name): VirtualBezierC0()
{
    p_Selected = Selectable::CreateRegisteredComponent(objectID);
    p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected);
    
    selectedNotifier = p_Selected->Selected.addNotifier([this](){
        if (p_Selected->Selected)
            CurveColor = QColor::fromRgbF(1.0f, 0.5f, 0.2f, 1.0f);
        else
            CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);
    });
}
