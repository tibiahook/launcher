#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "Initializer.h"

#define LAUNCHER_USAGE "tibia-launcher --library <library-path> --tibia <executable>"

#define ERROR_NONE 0
#define ERROR_ARGUMENTS 1
#define ERROR_INVALID 2

int main(int argc, char** argv) {
    QCoreApplication application(argc, argv);

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.length() < 5) {
        qDebug() << "usage: " LAUNCHER_USAGE;
        return ERROR_ARGUMENTS;
    }

    QString libraryPath;
    QString tibiaPath;
    for (int index = 0; index < arguments.length(); ++index) {
        if (arguments[index] == "--library") {
            libraryPath = arguments[++index];
        }
        else if (arguments[index] == "--tibia") {
            tibiaPath = arguments[++index];
        }
    }

    if (libraryPath.isEmpty() || tibiaPath.isEmpty()) {
        qDebug() << "usage: " LAUNCHER_USAGE;
        return ERROR_INVALID;
    }

    Initializer initializer(libraryPath, tibiaPath, &application);
    QTimer timer(&application);
    timer.setSingleShot(true);

    QObject::connect(&initializer, &Initializer::finished, &QCoreApplication::quit);
    QObject::connect(&timer, &QTimer::timeout, &initializer, &Initializer::start);

    // Causes the initializer to start when the event loop starts
    timer.start(0);

    // Start the event loop
    return application.exec();
}
