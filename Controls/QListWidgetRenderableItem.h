//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_QLISTWIDGETRENDERABLEITEM_H
#define SMOCAD_QLISTWIDGETRENDERABLEITEM_H


#include <QListWidgetItem>
#include "Objects/IRenderableObject.h"

class QListWidgetRenderableItem: public QListWidgetItem
{
private:
    IRenderableObject* obj;

public:
    QListWidgetRenderableItem(QListWidget* parent, const QString& name, IRenderableObject* obj);

    //friend operator==(QListWidgetRenderableItem one, QListWidgetRenderableItem two)

};


#endif //SMOCAD_QLISTWIDGETRENDERABLEITEM_H
