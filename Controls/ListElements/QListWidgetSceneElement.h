//
// Created by ksm on 4/17/22.
//

#ifndef SMOCAD_QLISTWIDGETSCENEELEMENT_H
#define SMOCAD_QLISTWIDGETSCENEELEMENT_H


#include <QListWidget>
#include "Scene/Components/SceneElement.h"

class QListWidgetSceneElement: public QListWidgetItem
{
private:
    std::weak_ptr<SceneElement> element;
    QPropertyNotifier nameNotifier;

public:
    QListWidgetSceneElement(QListWidget* parent, std::shared_ptr<SceneElement> element);

    void SelectItem();
    void Rename(const QString& name);
    unsigned int GetAttachedObjectID();
    const QString GetName();
};


#endif //SMOCAD_QLISTWIDGETSCENEELEMENT_H
