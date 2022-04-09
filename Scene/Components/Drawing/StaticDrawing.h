//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_STATICDRAWING_H
#define SMOCAD_STATICDRAWING_H


#include "Drawing.h"
class StaticDrawing: public Drawing
{
    Q_OBJECT
public:
    explicit StaticDrawing(unsigned int oid);
    static std::shared_ptr<StaticDrawing> CreateRegisteredComponent(unsigned int oid);

    void IntializeBuffers() override;
    void Render(QOpenGLContext* context) override;
};


#endif //SMOCAD_STATICDRAWING_H
