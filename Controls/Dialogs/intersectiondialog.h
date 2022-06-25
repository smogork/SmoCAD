#ifndef INTERSECTIONDIALOG_H
#define INTERSECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class IntersectionDialog;
}

class IntersectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntersectionDialog(QWidget *parent = nullptr);
    ~IntersectionDialog();

    float PointsSceneDistance() { return 0.1f; }//[TODO] dodac wyciaganie danych z UI
    bool StartFromCursor() { return false; }
    int SearchingStartDensity() { return 10; }

private:
    Ui::IntersectionDialog *ui;
};

#endif // INTERSECTIONDIALOG_H
