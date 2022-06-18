#include "transformcollectioncontrol.h"
#include "ui_transformcollectioncontrol.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"

#include <QMenu>

TransformCollectionControl::TransformCollectionControl(std::weak_ptr<TransformCollection> collection, QWidget *parent) :
        ComponentControl(parent),
        ui(new Ui::TransformCollectionControl)
{
    ui->setupUi(this);
    ui->listWidgetPoints->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidgetPoints, &QListWidget::customContextMenuRequested, this,
            &TransformCollectionControl::showObjectListContextMenu);


    m_collection = collection;
    UpdateElementsList();
    if (auto c = m_collection.lock())
    {
        connect(c.get(), &TransformCollection::PointInCollectionModified,
                this, &TransformCollectionControl::UpdateElementsList);

        if (c->IsContentLocked())
            ui->listWidgetPoints->setEnabled(false);
    }
}

TransformCollectionControl::~TransformCollectionControl()
{
    delete ui;
}

void TransformCollectionControl::UpdateElementsList()
{
    m_elements.clear();
    if (auto c = m_collection.lock())
    {
        if (auto scene = SceneECS::Instance().lock())
        {
            for (const std::weak_ptr<Transform> &wp: c->GetPoints())
            {
                auto sceneElement = scene->GetComponentOfSystem<SceneElementSystem, SceneElement>(
                        wp.lock()->GetAttachedObjectID()).lock();
                if (sceneElement)
                    m_elements.push_back(
                            std::make_unique<QListWidgetCollectionElement>(ui->listWidgetPoints, sceneElement));
            }
        }
    }
}

void TransformCollectionControl::showObjectListContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->listWidgetPoints->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    if (auto scene = SceneECS::Instance().lock())
    {
        myMenu.addAction("Remove from collection", this, [this]()
        {
            auto item = (QListWidgetCollectionElement *) (*ui->listWidgetPoints->selectedItems().begin());

            RemovePointFromCollection(item->GetAttachedObjectID());

            emit RequestRepaint();
        });
    }

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void TransformCollectionControl::RemovePointFromCollection(unsigned int oid)
{
    if (auto col = m_collection.lock())
        col->RemovePoint(oid);
}


