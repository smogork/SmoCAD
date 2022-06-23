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

private:
    Ui::IntersectionDialog *ui;
};

#endif // INTERSECTIONDIALOG_H
