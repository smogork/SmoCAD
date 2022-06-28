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

    QImage GetTrimmingTextureOne();
    QImage GetTrimmingTextureTwo();
    QImage GetSelfTrimmingTexture();
    QImage GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane);
    void FloodFill4(QPoint start, uint color, QImage& image, bool wrapX, bool wrapY);
    void DrawParametersPolylineOnTexture(QImage& image, std::shared_ptr<IntersectionAware> plane, const std::vector<QVector2D> &points);

public:


    static std::shared_ptr<IntersectionResult>
    CreateRegisteredComponent(unsigned int oid, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                              std::shared_ptr<IntersectionAware> two, bool isCycle);
    void UnregisterComponent();

    explicit IntersectionResult(unsigned int oid);
    ~IntersectionResult() override;

    std::vector<QVector3D> GetScenePoints();
    int GetScenePointsSize();

    std::tuple<QImage, QImage> GetTrimmingTextures();
};


#endif //SMOCAD_INTERSECTIONRESULT_H
