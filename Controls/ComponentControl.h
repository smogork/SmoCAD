//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_COMPONENTCONTROL_H
#define SMOCAD_COMPONENTCONTROL_H

#define UPDATE_VALUE_IGNORING_NOTIFIER(F)   if (ignoreValueChanged) return;\
                                            ignoreNotifier = true;\
                                            F;\
                                            ignoreNotifier = false;\
                                            emit RequestRepaint()

#include <QWidget>

class ComponentControl: public QWidget
{
    Q_OBJECT

protected:
    bool ignoreNotifier = false;
    bool ignoreValueChanged = true;

public:
    explicit ComponentControl(QWidget *parent = nullptr);

signals:
    void RequestRepaint();
};


#endif //SMOCAD_COMPONENTCONTROL_H
