//
// Created by ksm on 6/19/22.
//

#include "IntersectionAwareSystem.h"
#include "Controls/Dialogs/intersectiondialog.h"
#include "Mathematics/Optimization.h"
#include "Scene/SceneECS.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
IntersectionAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                            const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (filteredObjects.size() == 2)
        res.push_back(
                std::make_pair(
                        "Calculate intersection",
                        [this, filteredObjects](const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)
                        {
                            CreateIntersectionCurveBetween(filteredObjects[0], filteredObjects[1]);
                        }));
    
    if (filteredObjects.size() == 1)
        res.push_back(
                std::make_pair(
                        "Calculate self intersection",
                        [this, filteredObjects](const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)
                        {
                            CreateIntersectionCurveBetween(filteredObjects[0], filteredObjects[0]);
                        }));
    
    return res;
}

void IntersectionAwareSystem::CreateIntersectionCurveBetween(std::shared_ptr<IntersectionAware> one,
                                                             std::shared_ptr<IntersectionAware> two)
{
    std::unique_ptr<IntersectionDialog> dialog = std::make_unique<IntersectionDialog>();
    
    if (!dialog->exec())
        return;

    auto func = [one, two](QVector4D args) -> float
    {
        return (one->SceneFunction({args.x(), args.y()}) -
                two->SceneFunction({args.z(), args.w()})).length();
    };

    auto grad = [one, two](QVector4D args) -> QVector4D
    {
        QVector3D vec = one->SceneFunction({args.x(), args.y()}) -
                        two->SceneFunction({args.z(), args.w()});
        QVector4D ders = {
                QVector3D::dotProduct(vec, one->SceneFunctionDerU({args.x(), args.y()})),
                QVector3D::dotProduct(vec, one->SceneFunctionDerV({args.x(), args.y()})),
                QVector3D::dotProduct(vec, -two->SceneFunctionDerU({args.z(), args.w()})),
                QVector3D::dotProduct(vec, -two->SceneFunctionDerV({args.z(), args.w()})),
        };
        return 2.0f / vec.length() * ders;
    };

    QVector4D startPoint = one->FindClosestPoints(two);
    qDebug() << "Found startPoint " << startPoint;

    QVector4D P0_params = Optimization::SimpleGradientMethod(startPoint, func, grad);
    qDebug() << "Found P0_params " << P0_params;
    if (P0_params.x() == NAN or !one->ArgumentsInsideDomain(P0_params.toVector2D()) or !two->ArgumentsInsideDomain({P0_params.z(), startPoint.w()}))
        qDebug() << "Error with result " << P0_params;



    /*while ()

    auto newton_func1 = [one, two](QVector4D args) -> QVector4D
    {
        QVector3D vec = one->SceneFunction({args.x(), args.y()}) -
                        two->SceneFunction({args.z(), args.w()});
        return {
            vec.x(), vec.y(), vec.z(),

        }
    };

    auto newton_der1 = [one, two](QVector4D args) -> QMatrix4x4
    {
        QVector3D vec = one->SceneFunction({args.x(), args.y()}) -
                        two->SceneFunction({args.z(), args.w()});
        QVector4D ders = {
                QVector3D::dotProduct(vec, one->SceneFunctionDerU({args.x(), args.y()})),
                QVector3D::dotProduct(vec, one->SceneFunctionDerV({args.x(), args.y()})),
                QVector3D::dotProduct(vec, -two->SceneFunctionDerU({args.z(), args.w()})),
                QVector3D::dotProduct(vec, -two->SceneFunctionDerV({args.z(), args.w()})),
        };
        return 2.0f / vec.length() * ders;
    };

    Optimization::Eul*/


}

QVector4D IntersectionAwareSystem::FindFirstPointOfIntersection(std::shared_ptr<IntersectionAware> one,
                                                                std::shared_ptr<IntersectionAware> two)
{
    return QVector4D();
}

std::list<QVector4D> IntersectionAwareSystem::FindFurtherPointsOfIntersection(QVector4D P0, bool direction,
                                                                              std::shared_ptr<IntersectionAware> one,
                                                                              std::shared_ptr<IntersectionAware> two)
{
    return std::list<QVector4D>();
}
