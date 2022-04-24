//
// Created by ksm on 4/23/22.
//

#include "CustomQListWidget.h"

void CustomQListWidget::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::RightButton))
        m_rightClick = true;
    else
        m_rightClick = false;

    QListWidget::mousePressEvent(event);
}

bool CustomQListWidget::RightLastClicked()
{
    return m_rightClick;
}
