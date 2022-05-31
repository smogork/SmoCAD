//
// Created by ksm on 4/13/22.
//

#include "BezierC0.h"

BezierC0::BezierC0(const QString &name): VirtualBezierC0()
{
    p_Selected = Selectable::CreateRegisteredComponent(objectID);
    p_SceneElement = SceneElement::CreateRegisteredComponent(objectID, name, p_Selected);

    p_SceneElement->SerializeObject = ASSIGN_SERIALIZER_FUNCTION(&BezierC0::SerializingFunction);
    
    selectedNotifier = p_Selected->Selected.addNotifier([this](){
        if (p_Selected->Selected)
            CurveColor = QColor::fromRgbF(1.0f, 0.5f, 0.2f, 1.0f);
        else
            CurveColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);
    });
}

void BezierC0::SerializingFunction(MG1::Scene &scene)
{
    MG1::BezierC0 b0;
    b0.SetId(GetObjectID());
    b0.name = p_SceneElement->Name.value().toStdString();

    for (const auto& wel : p_Collection->GetPoints())
    {
        if (auto el = wel.lock())
            b0.controlPoints.emplace_back(MG1::PointRef(el->GetAttachedObjectID()));
    }

    scene.bezierC0.push_back(b0);
}
