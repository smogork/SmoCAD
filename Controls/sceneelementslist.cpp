#include "sceneelementslist.h"
#include "ui_sceneelementslist.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"
#include "Scene/Entities/Curves/BezierC0.h"
#include "Scene/Systems/CollectionAwareSystem.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Scene/Systems/CompositeAwareSystem.h"

#include <QMenu>
#include <QInputDialog>

SceneElementsList::SceneElementsList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SceneElementsList)
{
    ui->setupUi(this);
    if (auto scene = SceneECS::Instance().lock())
        if (auto sas = scene->GetSystem<SceneElementSystem>().lock())
            sas->AttachItemList(ui->listSceneElements);


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
    if (ui->listSceneElements->selectedItems().size() == 0)
        return;

    auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());
    unsigned int oid = item->GetAttachedObjectID();

    // Create menu and insert some actions
    QMenu myMenu;
    if (auto scene = SceneECS::Instance().lock())
    {
        myMenu.addAction("Remove", this, &SceneElementsList::onRemoveSceneElement);

        if (ui->listSceneElements->selectedItems().size() == 1)
        {
            myMenu.addAction("Rename", this, &SceneElementsList::onRenameSceneElement);
        }
        else if (ui->listSceneElements->selectedItems().size() > 1)
        {
            //[TODO] wyprowadzic to do systemu z kolekcjami - wymag aprzebudowania sposobu przekazywania zaznaczonych obiektów
            myMenu.addAction("Create BezierC0 from points", this, &SceneElementsList::CreateBezierC0);
            myMenu.addAction("Create BezierC2 from points", this, &SceneElementsList::CreateBezierC2);
            myMenu.addAction("Create InterpolationC2 from points", this, &SceneElementsList::CreateInterpolationC2);
        }

        auto menu_items = scene->CreateContextMenuForSceneElement(oid, ui->listSceneElements->selectedItems().size());
        for (const std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > &menu_item: menu_items)
        {
            const std::function<void(QListWidgetSceneElement *item)> &func = menu_item.second;
            myMenu.addAction(menu_item.first, this, [func, this]()
            {
                auto item = (QListWidgetSceneElement *) (*ui->listSceneElements->selectedItems().begin());

                if (func)
                    func(item);

                //emit RequestControlsUpdate(item->GetAttachedObjectID());
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

    if (ui->listSceneElements->selectedItems().size() == 1)
    {
        rItem->SelectItem();
        emit RequestControlsUpdate(rItem->GetAttachedObjectID());
        emit RequestRepaint();
    }
    else
    {
        if (auto scene = SceneECS::Instance().lock())
        {
            if (auto cas = scene->GetSystem<CompositeAwareSystem>().lock())
            {
                std::list<unsigned int> objs;
                for (QListWidgetItem *gElem: ui->listSceneElements->selectedItems())
                {
                    auto item = (QListWidgetSceneElement *) gElem;
                    objs.emplace_back(item->GetAttachedObjectID());
                }

                emit RequestControlsUpdate(cas->SelectMultipleObjects(objs));
                emit RequestRepaint();
            }
        }
    }
}

void SceneElementsList::onRemoveSceneElement()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        for (QListWidgetItem *gElem: ui->listSceneElements->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            scene->RemoveObject(item->GetAttachedObjectID());
        }
    }

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

void SceneElementsList::CreateBezierC0()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC0> b0 = std::make_shared<BezierC0>("NewBezierC0");

        int itemsAdded = 0;
        for (QListWidgetItem *gElem: ui->listSceneElements->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                b0->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }

        if (itemsAdded)
        {
            scene->AddObject(b0);

            b0->p_Selected->Selected = true;
            emit RequestControlsUpdate(b0->GetObjectID());
            emit RequestRepaint();
        }
    }
}

void SceneElementsList::CreateBezierC2()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC2> b2 = std::make_shared<BezierC2>("NewBezierC2");

        int itemsAdded = 0;
        for (QListWidgetItem *gElem: ui->listSceneElements->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                b2->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }

        if (itemsAdded)
        {
            scene->AddObject(b2);

            b2->p_Selected->Selected = true;
            emit RequestControlsUpdate(b2->GetObjectID());
            emit RequestRepaint();
        }
    }

}

void SceneElementsList::CreateInterpolationC2()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("NewInterpolationC2");

        int itemsAdded = 0;
        for (QListWidgetItem *gElem: ui->listSceneElements->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                i2->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }

        if (itemsAdded)
        {
            scene->AddObject(i2);

            i2->p_Selected->Selected = true;
            emit RequestControlsUpdate(i2->GetObjectID());
            emit RequestRepaint();
        }
    }

}
