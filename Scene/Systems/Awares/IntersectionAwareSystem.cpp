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


    QVector4D P0 = FindFirstPointOfIntersection(one, two);

    bool edgeEnd;
    std::list<QVector4D> negative_points, positive_points = FindFurtherPointsOfIntersection(P0,
                                                                                            dialog->PointsSceneDistance(),
                                                                                            true,
                                                                                            edgeEnd, one, two);
    if (edgeEnd)//puszczamy w druga strone tylko jesli nie trafilismy na cykl
        negative_points = FindFurtherPointsOfIntersection(P0, dialog->PointsSceneDistance(), false,
                                                          edgeEnd, one, two);

    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObjectExplicitPosition(
                std::make_shared<Point>("IntertsectionP0", one->SceneFunction(P0.toVector2D())));

        int i = 0;
        for (const auto &params: positive_points)
            scene->AddObjectExplicitPosition(std::make_shared<Point>(QString("PositivePoints%0").arg(i++),
                                                                     one->SceneFunction(params.toVector2D())));
        for (const auto &params: negative_points)
            scene->AddObjectExplicitPosition(std::make_shared<Point>(QString("NegativePoints%0").arg(i++),
                                                                     one->SceneFunction(params.toVector2D())));
    }
}

QVector4D IntersectionAwareSystem::FindFirstPointOfIntersection(std::shared_ptr<IntersectionAware> one,
                                                                std::shared_ptr<IntersectionAware> two)
{
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
    if (P0_params.x() == NAN or !one->ArgumentsInsideDomain(P0_params.toVector2D()) or
        !two->ArgumentsInsideDomain({P0_params.z(), startPoint.w()}))
        qDebug() << "Error with result " << P0_params;

    return P0_params;
}

std::list<QVector4D>
IntersectionAwareSystem::FindFurtherPointsOfIntersection(QVector4D P0, float dist, bool direction, bool &edgeEnd,
                                                         std::shared_ptr<IntersectionAware> one,
                                                         std::shared_ptr<IntersectionAware> two)
{
    QVector4D cur_point = P0, last_point;
    QVector3D N, last_value;
    float params_dist = 0.001;
    std::list<QVector4D> res;
    edgeEnd = false;

    do
    {
        //przepisanie aktualnej wartosci jako starej
        last_point = cur_point;
        last_value = one->SceneFunction({last_point.x(), last_point.y()});

        //obliczenie wektora wzdluz ktorego bedziemy szukac punktu

        auto nf =QVector3D::crossProduct(
                        one->SceneFunctionDerU({last_point.x(), last_point.y()}),
                        one->SceneFunctionDerV({last_point.x(), last_point.y()}));
        auto ng =QVector3D::crossProduct(
                        two->SceneFunctionDerU({last_point.z(), last_point.w()}),
                        two->SceneFunctionDerV({last_point.z(), last_point.w()}));

        QVector3D N = QVector3D::crossProduct(nf, ng).normalized();
        if (direction)
            N *= -1;

        //definicja funkcji majacej zero na przecieciu w odpowiedniej odleglosci
        auto func = [one, two, &last_value, &N, &dist](QVector4D args) -> QVector4D
        {
            QVector3D vec = one->SceneFunction({args.x(), args.y()}) -
                            two->SceneFunction({args.z(), args.w()});
            return
                    {
                            vec.x(), vec.y(), vec.z(),
                            QVector3D::dotProduct(one->SceneFunction({args.x(), args.y()}) - last_value, N) - dist
                    };
        };

        //pochodna funkcji powyzej
        auto der = [one, two, &N](QVector4D args) -> QMatrix4x4
        {
            QVector3D fdu = one->SceneFunctionDerU({args.x(), args.y()});
            QVector3D fdv = one->SceneFunctionDerV({args.x(), args.y()});
            QVector3D gdu = two->SceneFunctionDerU({args.z(), args.w()});
            QVector3D gdv = two->SceneFunctionDerV({args.z(), args.w()});

            return {
                    fdu.x(), fdv.x(), -gdu.x(), -gdv.x(),
                    fdu.y(), fdv.y(), -gdu.y(), -gdv.y(),
                    fdu.z(), fdv.z(), -gdu.z(), -gdv.z(),
                    QVector3D::dotProduct(fdu, N), QVector3D::dotProduct(fdv, N), 0, 0
            };
        };

        auto domainCheck = [one, two](QVector4D args) -> bool
        {
            return one->ArgumentsInsideDomain({args.x(), args.y()}) &&
                two->ArgumentsInsideDomain({args.z(), args.w()});
        };

        auto domainClamp = [one, two, &N](QVector4D args, QVector4D last) -> QVector4D
        {
            //[TODO] przyciac na lini pomiedzy last a args
            return args;
        };


        //znalezienie nowego punktu na przecieciu
        cur_point = Optimization::NewtonRaphsonMethod(last_point, func, der, domainCheck, domainClamp, edgeEnd, 1e-3, 200);


        if (cur_point.x() == NAN)
        {
            qDebug() << "Error for params:" << last_point;
            break;
        }

        //Liczymy doc zasu jak nie zblizymy sie zbyt do punktu startowego na scenie oraz w dziedzinie parametrow
        if (one->SceneFunction(cur_point.toVector2D()).distanceToPoint(
                two->SceneFunction({cur_point.z(), cur_point.w()})) < dist &&
            (cur_point - P0).length() < params_dist)
            break;

        res.push_back(cur_point);
        qDebug() << "Found next point H(" << cur_point << ") = " << one->SceneFunction({cur_point.x(), cur_point.y()});
    } while (!edgeEnd);

    qDebug() << "Found " << res.size() << "points";
    qDebug() << "EdgeEnd:" << edgeEnd;
    return res;
}
