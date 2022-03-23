//
// Created by ksm on 3/22/22.
//

#include "QListWidgetRenderableItem.h"

QListWidgetRenderableItem::QListWidgetRenderableItem(QListWidget *parent, const QString& name, IRenderableObject *obj, std::shared_ptr<SceneModel> scene)
: QListWidgetItem(name, parent)
{
    this->obj = obj;
    this->scene = scene;
}

void QListWidgetRenderableItem::SelectOnScene(bool append)
{
    if (append)
        scene->AppendToSelectedObjects(obj);
    else
        scene->SelectObject(obj);
}

bool QListWidgetRenderableItem::CompareInsideObject(const IRenderableObject* other)
{
    return obj == other;
}
