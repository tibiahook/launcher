#ifndef WINDOWSINJECTOR_H
#define WINDOWSINJECTOR_H

#include "Injector.h"

#include <QProcess>
#include <QTimer>

class WindowsInjector: public Injector {
    Q_OBJECT

public:
    WindowsInjector(const QString& libraryPath, QObject* parent = 0);

    bool startAndAttach(const QString& application, const QStringList& arguments, const QString& workingDirectory = QString());
    bool attach(Q_PID processId);

private slots:
    void printStandardOutput();
    void printStandardError();

    void attachToSpawnedProcess();

private:
    bool copyAdjustedPluginSettings(const QString &source, const QString &destination);

    QProcess* process_;
    QTimer* attachTimer_;
};

#endif
