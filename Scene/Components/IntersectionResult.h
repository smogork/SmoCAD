//
// Created by ksm on 6/28/22.
//

#ifndef SMOCAD_INTERSECTIONRESULT_H
#define SMOCAD_INTERSECTIONRESULT_H


#include "IComponent.h"
#include "Scene/Components/Awares/IntersectionAware.h"

class IntersectionResult : public IComponent
{
protected:
    std::vector<QVector4D> m_paramPoints;
    std::shared_ptr<IntersectionAware> m_planeOne, m_planeTwo;
    bool m_cycle;

    QImage GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane);
    void FloodFill4(QPoint start, uint color, QImage& image, bool wrapX, bool wrapY);

public:

    static std::shared_ptr<IntersectionResult>
    CreateRegisteredComponent(unsigned int oid, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                              std::shared_ptr<IntersectionAware> two, bool isCycle);
    void UnregisterComponent();

    explicit IntersectionResult(unsigned int oid);
    ~IntersectionResult() override;

    std::vector<QVector3D> GetScenePoints();
    int GetScenePointsSize();

    QImage GetTrimmingTextureOne();
    QImage GetTrimmingTextureTwo();
};


#endif //SMOCAD_INTERSECTIONRESULT_H
