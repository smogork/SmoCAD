//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_COMPONENTCONTROL_H
#define SMOCAD_COMPONENTCONTROL_H


#include <QWidget>

class ComponentControl: public QWidget
{
    Q_OBJECT
public:
    explicit ComponentControl(QWidget *parent = nullptr);

signals:
    void RequestRepaint();
};


#endif //SMOCAD_COMPONENTCONTROL_H
