#ifndef INTERSECTIONRESULTCONTROL_H
#define INTERSECTIONRESULTCONTROL_H

#include <QWidget>
#include "Scene/Components/IntersectionResult.h"
#include "ComponentControl.h"
#include "Controls/Dialogs/parametersintersectiondialog.h"

namespace Ui {
class IntersectionResultControl;
}

class IntersectionResultControl : public ComponentControl
{
    Q_OBJECT
public:
    explicit IntersectionResultControl(std::shared_ptr<IntersectionResult> res, QWidget *parent = nullptr);
    ~IntersectionResultControl();

private slots:
    void on_btnDomains_clicked();
    void on_btnInterpolation_clicked();

private:
    Ui::IntersectionResultControl *ui;
    std::weak_ptr<IntersectionResult> m_res;
    std::unique_ptr<ParametersIntersectionDialog> paramsDialog;
};

#endif // INTERSECTIONRESULTCONTROL_H
