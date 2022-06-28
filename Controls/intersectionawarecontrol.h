#ifndef INTERSECTIONAWARECONTROL_H
#define INTERSECTIONAWARECONTROL_H

#include <QWidget>
#include "Scene/Components/Awares/IntersectionAware.h"
#include "ComponentControl.h"


namespace Ui {
class IntersectionAwareControl;
}

class IntersectionAwareControl : public ComponentControl
{
    Q_OBJECT

public:
    explicit IntersectionAwareControl(std::shared_ptr<IntersectionAware> aware, QWidget *parent = nullptr);
    ~IntersectionAwareControl();

private slots:
    void on_btnFlip_clicked();

private:
    Ui::IntersectionAwareControl *ui;
    std::weak_ptr<IntersectionAware> m_aware;

};

#endif // INTERSECTIONAWARECONTROL_H
