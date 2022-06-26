//
// Created by ksm on 6/19/22.
//

#include "IntersectionAwareSystem.h"
#include "Controls/Dialogs/intersectiondialog.h"
#include "Mathematics/Optimization.h"
#include "Scene/SceneECS.h"
#include "Scene/Entities/Curves/IntersectionCurve.h"
#include "Controls/EntityContextMenu.h"

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

    bool edgeEndP, edgeEndN;
    std::list<QVector4D> negative_points;
    std::list<QVector4D> positive_points = FindFurtherPointsOfIntersection(P0, dialog->PointsSceneDistance(),
                                                                           true, edgeEndP, one, two);
    if (edgeEndP)//puszczamy w druga strone tylko jesli nie trafilismy na cykl
        negative_points = FindFurtherPointsOfIntersection(P0, dialog->PointsSceneDistance(), false,
                                                          edgeEndN, one, two);

    qDebug() << "Found P0:" << P0 << ", positives:" << positive_points.size() << ", negatives:" << negative_points.size();
    qDebug() << "edgeEndP:" << edgeEndP << ", edgeEndN:" << edgeEndN;

    if (auto scene = SceneECS::Instance().lock())
    {
        std::vector<QVector2D> intersectionArgs;
        intersectionArgs.reserve(positive_points.size() + negative_points.size() + 1);

        for (auto it = positive_points.rbegin(); it != positive_points.rend(); ++it)
            intersectionArgs.push_back((*it).toVector2D());
        intersectionArgs.push_back(P0.toVector2D());
        for (const auto& arg : negative_points)
            intersectionArgs.push_back(arg.toVector2D());

        auto curve = std::make_shared<IntersectionCurve>("IntersectionCurve", intersectionArgs, one->SceneFunction, !edgeEndP && !edgeEndN);
        curve->p_Selected->Selected = true;

        scene->AddObject(curve);

        EntityContextMenu::MakeControlsUpdate(curve->GetObjectID());
        EntityContextMenu::MakeRepaint();
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
    QVector4D P0_params = Optimization::SimpleGradientMethod(startPoint, func, grad);

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

    auto domainCheck = [one, two](QVector4D args) -> bool
    {
        return one->ArgumentsInsideDomain({args.x(), args.y()}) &&
               two->ArgumentsInsideDomain({args.z(), args.w()});
    };

    auto domainClamp = [one, two](QVector4D args, QVector4D last) -> QVector4D
    {
        QVector4D delta = args - last;
        float minChange = 1.0f;

        if (delta.x())
            minChange = std::min(minChange, (std::clamp<float>(args.x(), one->UMin, one->UMax) - last.x()) / delta.x());
        if (delta.y())
            minChange = std::min(minChange, (std::clamp<float>(args.y(), one->VMin, one->VMax) - last.y()) / delta.y());
        if (delta.z())
            minChange = std::min(minChange, (std::clamp<float>(args.z(), two->UMin, two->UMax) - last.z()) / delta.z());
        if (delta.w())
            minChange = std::min(minChange, (std::clamp<float>(args.w(), two->VMin, two->VMax) - last.w()) / delta.w());

        return last + delta * minChange;
    };

    do
    {
        //przepisanie aktualnej wartosci jako starej
        last_point = cur_point;
        last_value = one->SceneFunction({last_point.x(), last_point.y()});

        //obliczenie wektora wzdluz ktorego bedziemy szukac punktu

        auto nf = QVector3D::crossProduct(
                one->SceneFunctionDerU({last_point.x(), last_point.y()}),
                one->SceneFunctionDerV({last_point.x(), last_point.y()}));
        auto ng = QVector3D::crossProduct(
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


        //znalezienie nowego punktu na przecieciu
        cur_point = Optimization::NewtonRaphsonMethod(last_point, func, der, domainCheck, domainClamp, edgeEnd, 1e-3,
                                                      200);

        if (cur_point.x() == NAN)
        {
            qDebug() << "Error for params:" << last_point;
            break;
        }

        auto temp1 = one->SceneFunction(cur_point.toVector2D());
        auto temp2 = one->SceneFunction(P0.toVector2D());
        //Liczymy doc zasu jak nie zblizymy sie zbyt do punktu startowego na scenie oraz w dziedzinie parametrow
        if (temp1.distanceToPoint(temp2) < dist * 0.7f) /*&&
            (cur_point - P0).length() < params_dist)*/
            break;

        res.push_back(cur_point);
    } while (!edgeEnd);

    return res;
}
