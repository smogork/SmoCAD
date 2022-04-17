#ifndef TRANSFORMCOLLECTIONCONTROL_H
#define TRANSFORMCOLLECTIONCONTROL_H

#include <QWidget>
#include <QListWidgetItem>
#include <QPropertyNotifier>
#include "ComponentControl.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Components/SceneElement.h"
#include "Controls/ListElements/QListWidgetCollectionElement.h"

namespace Ui
{
    class TransformCollectionControl;
}

class TransformCollectionControl : public ComponentControl
{
Q_OBJECT

public:
    explicit TransformCollectionControl(std::weak_ptr<TransformCollection> collection, QWidget *parent = nullptr);
    ~TransformCollectionControl();

private:
    Ui::TransformCollectionControl *ui;
    std::weak_ptr<TransformCollection> m_collection;
    std::list<std::unique_ptr<QListWidgetCollectionElement>> m_elements;

private slots:
    void UpdateElementsList();
};

#endif // TRANSFORMCOLLECTIONCONTROL_H
