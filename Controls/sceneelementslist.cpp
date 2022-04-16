#include "sceneelementslist.h"
#include "ui_sceneelementslist.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"

#include <QMenu>

SceneElementsList::SceneElementsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneElementsList)
{
    ui->setupUi(this);
    SceneECS::elementList = ui->listSceneElements;

    ui->listSceneElements->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listSceneElements, &QListWidget::customContextMenuRequested, this, &SceneElementsList::showObjectListContextMenu);
}

SceneElementsList::~SceneElementsList()
{
    delete ui;
}

void SceneElementsList::showObjectListContextMenu(const QPoint &pos) {
    QPoint globalPos = ui->listSceneElements->mapToGlobal(pos);

    int selectedPoints = 0;
    for (QListWidgetItem* i : ui->listSceneElements->selectedItems())
        selectedPoints += dynamic_cast<SceneElementSystem::QListWidgetSceneElement*>(i) ? 1 : 0;

    // Create menu and insert some actions
    QMenu myMenu;
    if (auto scene = SceneECS::Instance().lock())
    {
        /*myMenu.addAction("Remove", this, &MainWindow::on_actionDelete_triggered);
        if (scene->GetSystem<TransformCollectionSystem>().lock()->GetComponent())
            myMenu.addAction("Add to bezier", this, &MainWindow::AddPointToBezier);
        if (selectedPoints > 1)
            myMenu.addAction("Create bezier from points", this, &MainWindow::CreateBezierFromPoints);
        //myMenu.addAction("Erase",  this, SLOT(eraseItem()));*/
    }

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void SceneElementsList::on_listSceneElements_itemClicked(QListWidgetItem *item)
{
    auto rItem = (SceneElementSystem::QListWidgetSceneElement*)item;

    if (ui->listSceneElements->selectedItems().size() == 1)
    {
        rItem->SelectItem();
        emit RequestControlsUpdate(rItem->GetAttachedObjectID());
        emit RequestRepaint();
    }
}

