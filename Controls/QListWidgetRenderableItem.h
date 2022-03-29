//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_QLISTWIDGETRENDERABLEITEM_H
#define SMOCAD_QLISTWIDGETRENDERABLEITEM_H


#include <QListWidgetItem>
#include "Objects/IRenderableObject.h"
#include "Scene/SceneModel.h"

class QListWidgetRenderableItem: public QListWidgetItem
{
private:

    std::shared_ptr<SceneModel> scene;

public:
    //[TODO] to jest do przneiesienia na private. tymczasowe rozwaizanie dla sprawdzenia czy przypoadkiem nie ma torusa tutaj
    IRenderableObject* obj;

    QListWidgetRenderableItem(QListWidget* parent, const QString& name, IRenderableObject* obj, std::shared_ptr<SceneModel> scene);

    bool SelectOnScene(bool append = false);
    bool CompareInsideObject(const IRenderableObject* other);

    //friend operator==(QListWidgetRenderableItem one, QListWidgetRenderableItem two)

};


#endif //SMOCAD_QLISTWIDGETRENDERABLEITEM_H
