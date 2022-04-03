#include "mainwindow.h"
#include <iostream>

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "Scene/Entities/Point.h"
#include "Scene/SceneECS.h"

void test()
{

    {
        auto transformSys = SceneECS::Instance().lock()->GetSystem<TransformSystem>();
        std::cout << transformSys.lock()->GetComponents().size() << std::endl;

        Point p1((QVector3D(1, 0, 0)));
        {
            auto transformSys = SceneECS::Instance().lock()->GetSystem<TransformSystem>();
            std::cout << transformSys.lock()->GetComponents().size() << std::endl;

            Point p2((QVector3D(0, 1, 0))), p3((QVector3D(0, 0, 1)));

            transformSys = SceneECS::Instance().lock()->GetSystem<TransformSystem>();
            std::cout << transformSys.lock()->GetComponents().size() << std::endl;
        }
        transformSys = SceneECS::Instance().lock()->GetSystem<TransformSystem>();
        std::cout << transformSys.lock()->GetComponents().size() << std::endl;
    }
    auto transformSys = SceneECS::Instance().lock()->GetSystem<TransformSystem>();
    std::cout << transformSys.lock()->GetComponents().size() << std::endl;
}

int main(int argc, char *argv[])
{
    /*QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SmoCAD_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();*/
    test();
}
