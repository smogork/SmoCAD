//
// Created by ksm on 6/28/22.
//

#ifndef SMOCAD_INTERSECTIONRESULT_H
#define SMOCAD_INTERSECTIONRESULT_H


#include "IComponent.h"
#include "Scene/Components/Awares/IntersectionAware.h"

class IntersectionResult : public IComponent
{
    Q_OBJECT
private :
    static constexpr QRgb c_curve = qRgb(0, 255, 0);
    static constexpr QRgb c_free = qRgb(255, 255, 255);
    static constexpr QRgb c_flood = qRgb(0, 0, 0);

protected:
    std::vector<QVector4D> m_paramPoints;
    std::weak_ptr<IntersectionAware> m_planeOne, m_planeTwo;
    bool m_cycle;

    QImage GetTrimmingTextureOne();
    QImage GetTrimmingTextureTwo();
    QImage GetSelfTrimmingTexture();
    QImage GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane);
    void FloodFill4(QPoint start, QRgb color, QImage& image, bool wrapX, bool wrapY);
    void DrawParametersPolylineOnTexture(QRgb color, QImage& image, std::shared_ptr<IntersectionAware> plane, const std::vector<QVector2D> &points);

public:
    static std::shared_ptr<IntersectionResult>
    CreateRegisteredComponent(unsigned int oid, std::vector<QVector4D>& points, std::shared_ptr<IntersectionAware> one,
                              std::shared_ptr<IntersectionAware> two, bool isCycle);
    void UnregisterComponent();

    explicit IntersectionResult(unsigned int oid);
    ~IntersectionResult() override;

    std::vector<QVector3D> GetScenePoints();
    std::vector<QVector2D> GetFirstParameterPoints();
    std::vector<QVector2D> GetSecondParameterPoints();
    int GetScenePointsSize();

    std::tuple<QImage, QImage> GetTrimmingTextures();
};


#endif //SMOCAD_INTERSECTIONRESULT_H
