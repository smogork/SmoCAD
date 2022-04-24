//
// Created by ksm on 4/17/22.
//

#ifndef SMOCAD_QLISTWIDGETCOLLECTIONELEMENT_H
#define SMOCAD_QLISTWIDGETCOLLECTIONELEMENT_H


#include <QListWidgetItem>
#include "Scene/Components/SceneElement.h"

class QListWidgetCollectionElement : public QListWidgetItem
{
private:
    std::weak_ptr<SceneElement> element;
    QPropertyNotifier nameNotifier;

public:
    QListWidgetCollectionElement(QListWidget *parent, std::shared_ptr<SceneElement> element);

    unsigned int GetAttachedObjectID();
};


#endif //SMOCAD_QLISTWIDGETCOLLECTIONELEMENT_H
