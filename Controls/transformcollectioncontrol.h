#ifndef TRANSFORMCOLLECTIONCONTROL_H
#define TRANSFORMCOLLECTIONCONTROL_H

#include <QWidget>
#include <QListWidgetItem>
#include <QPropertyNotifier>
#include "ComponentControl.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Components/SceneElement.h"

namespace Ui
{
    class TransformCollectionControl;
}

class QListWidgetCollectionElement : public QListWidgetItem
{
private:
    std::weak_ptr<SceneElement> element;
    QPropertyNotifier nameNotifier;

public:
    QListWidgetCollectionElement(QListWidget *parent, std::shared_ptr<SceneElement> element);

    unsigned int GetAttachedObjectID();
};

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
