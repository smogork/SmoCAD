#include "sceneelementslist.h"
#include "ui_sceneelementslist.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"

#include <QMenu>
#include <QInputDialog>

SceneElementsList::SceneElementsList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SceneElementsList)
{
    ui->setupUi(this);
    SceneECS::elementList = ui->listSceneElements;

    ui->listSceneElements->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listSceneElements, &QListWidget::customContextMenuRequested, this,
            &SceneElementsList::showObjectListContextMenu);
}

SceneElementsList::~SceneElementsList()
{
    delete ui;
}

void SceneElementsList::showObjectListContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->listSceneElements->mapToGlobal(pos);
    auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());
    unsigned int oid = item->GetAttachedObjectID();

    /*int selectedPoints = 0;
    for (QListWidgetItem* i : ui->listSceneElements->selectedItems())
        selectedPoints += dynamic_cast<SceneElementSystem::QListWidgetSceneElement*>(i) ? 1 : 0;*/

    // Create menu and insert some actions
    QMenu myMenu;
    if (auto scene = SceneECS::Instance().lock())
    {
        myMenu.addAction("Rename", this, &SceneElementsList::onRenameSceneElement);
        myMenu.addAction("Remove", this, &SceneElementsList::onRemoveSceneElement);

        auto menu_items = scene->CreateContextMenuForSceneElement(oid);
        for (const std::pair<QString, std::function<void(QListWidgetSceneElement* item)> > &menu_item: menu_items)
        {
            const std::function<void(QListWidgetSceneElement* item)> &func = menu_item.second;
            myMenu.addAction(menu_item.first, this, [func, this]()
            {
                auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());

                if (func)
                    func(item);

                emit RequestControlsUpdate(item->GetAttachedObjectID());
                emit RequestRepaint();
            });
        }
    }

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void SceneElementsList::on_listSceneElements_itemClicked(QListWidgetItem *item)
{
    auto rItem = (QListWidgetSceneElement *) item;

    //if (ui->listSceneElements->selectedItems().size() == 1)
    //{
    rItem->SelectItem();
    emit RequestControlsUpdate(rItem->GetAttachedObjectID());
    emit RequestRepaint();
    //}
}

void SceneElementsList::onRemoveSceneElement()
{
    auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());

    if (auto scene = SceneECS::Instance().lock())
        scene->RemoveObject(item->GetAttachedObjectID());

    emit RequestControlsUpdate(SceneECS::NON_OBJECT_ID);
    emit RequestRepaint();
}

void SceneElementsList::onRenameSceneElement()
{
    auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());

    bool ok;
    QString newName = QInputDialog::getText(this, "Rename object", "Insert new name of object", QLineEdit::Normal,
                                            item->GetName(), &ok);
    if (!ok)
        return;

    item->Rename(newName);
}

