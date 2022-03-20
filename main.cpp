#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

/*
 * Cele na najblizszy czas:
 * 1. Narysowac 2 kostki
 * 2. Dodac obiekt kursor i narysowac obok kostek
 * 3. ?
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
    return a.exec();
}
