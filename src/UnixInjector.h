#ifndef UNIXINJECTOR_H
#define UNIXINJECTOR_H

#include "Injector.h"

#include <QProcess>

class UnixInjector: public Injector {
    Q_OBJECT

public:
    UnixInjector(const QString& libraryPath, QObject* parent = 0);

    bool startAndAttach(const QString& application, const QStringList& arguments, const QString& workingDirectory = QString());

private slots:
    void printStandardOutput();
    void printStandardError();

private:
    bool copyAdjustedPluginSettings(const QString& source, const QString& destination);

    QProcess* process_;
};

#endif
