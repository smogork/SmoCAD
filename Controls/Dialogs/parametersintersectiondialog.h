#ifndef PARAMETERSINTERSECTIONDIALOG_H
#define PARAMETERSINTERSECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ParametersIntersectionDialog;
}

class ParametersIntersectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParametersIntersectionDialog(QWidget *parent = nullptr);
    ~ParametersIntersectionDialog();

    void SetParamTextures(QImage& one, QImage& two);

private:
    Ui::ParametersIntersectionDialog *ui;
};

#endif // PARAMETERSINTERSECTIONDIALOG_H
