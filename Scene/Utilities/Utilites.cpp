//
// Created by ksm on 4/10/22.
//

#include "Utilites.h"

QVector4D ColorToVector4D(QColor c)
{
    return {c.redF(), c.greenF(), c.blueF(), c.alphaF()};
}