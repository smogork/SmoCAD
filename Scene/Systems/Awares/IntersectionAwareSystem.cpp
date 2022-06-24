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
    
    if (dialog->exec())
    {
        auto func = [one, two](QVector4D args) -> float
        {
            return (one->p_UV->SceneFunction({args.x(), args.y()}) -
                    two->p_UV->SceneFunction({args.z(), args.w()})).length();
        };
        
        auto grad = [one, two](QVector4D args) -> QVector4D
        {
            QVector3D vec = one->p_UV->SceneFunction({args.x(), args.y()}) -
                            two->p_UV->SceneFunction({args.z(), args.w()});
            QVector4D ders = {
                    QVector3D::dotProduct(vec, one->p_UV->SceneFunctionDerU({args.x(), args.y()})),
                    QVector3D::dotProduct(vec, one->p_UV->SceneFunctionDerV({args.x(), args.y()})),
                    QVector3D::dotProduct(vec, -two->p_UV->SceneFunctionDerU({args.z(), args.w()})),
                    QVector3D::dotProduct(vec, -two->p_UV->SceneFunctionDerV({args.z(), args.w()})),
            };
            return 2.0f / vec.length() * ders;
        };
        
        QVector4D startPoint = FindClosestPoints(one, two);
        
        QVector4D P0_params = Optimization::SimpleGradientMethod(startPoint, func, grad);
        if (P0_params.x() != NAN)
        {
            QVector3D P0 = one->p_UV->SceneFunction(P0_params.toVector2D());
            if (auto scene = SceneECS::Instance().lock())
                scene->AddObjectExplicitPosition(std::make_shared<Point>("IntersectionP0", P0));
        }
        else
            qDebug() << "Error with result " << P0_params;
    }
}

QVector4D IntersectionAwareSystem::FindClosestPoints(std::shared_ptr<IntersectionAware> one,
                                                     std::shared_ptr<IntersectionAware> two)
{
    //[TODO] zaimplementowac
    return QVector4D();
}
