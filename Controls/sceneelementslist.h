#ifndef SCENEELEMENTSLIST_H
#define SCENEELEMENTSLIST_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class SceneElementsList;
}

class SceneElementsList : public QWidget
{
    Q_OBJECT

public:
    explicit SceneElementsList(QWidget *parent = nullptr);
    ~SceneElementsList();

signals:
    void RequestRepaint();
    void RequestControlsUpdate(unsigned int oid);

private slots:
    void showObjectListContextMenu(const QPoint& pos);

    void on_listSceneElements_itemClicked(QListWidgetItem *item);

    //contex menu slots
    void onRemoveSceneElement();
    void onRenameSceneElement();
    void CreateBezierC0();
    void CreateBezierC2();

private:
    Ui::SceneElementsList *ui;
};

#endif // SCENEELEMENTSLIST_H
