//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_QLISTWIDGETRENDERABLEITEM_H
#define SMOCAD_QLISTWIDGETRENDERABLEITEM_H


#include <QListWidgetItem>
#include "Objects/IRenderableObject.h"
#include "SceneModel.h"

class QListWidgetRenderableItem: public QListWidgetItem
{
private:
    IRenderableObject* obj;
    std::shared_ptr<SceneModel> scene;

public:
    QListWidgetRenderableItem(QListWidget* parent, const QString& name, IRenderableObject* obj, std::shared_ptr<SceneModel> scene);

    void SelectOnScene();
    bool CompareInsideObject(const IRenderableObject* other);
    //friend operator==(QListWidgetRenderableItem one, QListWidgetRenderableItem two)

};


#endif //SMOCAD_QLISTWIDGETRENDERABLEITEM_H
