//
// Created by ksm on 4/8/22.
//

#ifndef SMOCAD_SELECTIONCHANGED_H
#define SMOCAD_SELECTIONCHANGED_H


#include <QEvent>

class SelectionChanged: QEvent
{
public:
    bool Selected;

    SelectionChanged(bool val): QEvent(User), Selected(val) { }
};


#endif //SMOCAD_SELECTIONCHANGED_H
