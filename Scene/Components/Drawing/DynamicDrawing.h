//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_DYNAMICDRAWING_H
#define SMOCAD_DYNAMICDRAWING_H


#include "StaticDrawing.h"

class DynamicDrawing: public StaticDrawing
{
    Q_OBJECT
protected:
    bool m_toUpdate = false;

    void UpdateBuffers();

public:
    explicit DynamicDrawing(unsigned int oid);
    static std::shared_ptr<DynamicDrawing> CreateRegisteredComponent(unsigned int oid);

    void Render(QOpenGLContext* context) override;
    void SetVertexData(std::vector<float> data) override;
    void SetIndexData(std::vector<int> data) override;
};


#endif //SMOCAD_DYNAMICDRAWING_H
