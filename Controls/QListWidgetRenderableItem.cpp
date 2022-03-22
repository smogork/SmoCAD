//
// Created by ksm on 3/22/22.
//

#include "QListWidgetRenderableItem.h"

QListWidgetRenderableItem::QListWidgetRenderableItem(QListWidget *parent, const QString& name, IRenderableObject *obj)
: QListWidgetItem(name, parent)
{
    this->obj = obj;
}
