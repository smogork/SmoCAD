//
// Created by ksm on 4/23/22.
//

#ifndef SMOCAD_CUSTOMQLISTWIDGET_H
#define SMOCAD_CUSTOMQLISTWIDGET_H

#include <QListWidget>
#include <QMouseEvent>

class CustomQListWidget: public QListWidget
{
public:
    CustomQListWidget(QWidget* parent = nullptr): QListWidget(parent) {}
    bool RightLastClicked();

private:
    bool m_rightClick = false;

    void mousePressEvent(QMouseEvent *event);
};

#endif //SMOCAD_CUSTOMQLISTWIDGET_H
