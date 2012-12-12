#include "UnixInjector.h"

#include <stdlib.h>

UnixInjector::UnixInjector(const QString& libraryPath, QObject* parent):
    Injector(libraryPath, parent),
    process_(NULL) {
}

bool UnixInjector::startAndAttach(const QString& application, const QStringList& arguments, const QString& workingDirectory) {
    // Clean up old process first
    if (process_ != NULL) {
        delete process_;
    }

    process_ = new QProcess(this);

    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), SIGNAL(finished(int)));

    QStringList libraries;
    libraries << "/usr/lib/i386-linux-gnu/libQtCore.so";
    libraries << "/usr/lib/i386-linux-gnu/libQtGui.so";
    libraries << libraryPath_;

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("LD_PRELOAD", libraries.join(":"));

    process_->setProcessEnvironment(environment);
    process_->setWorkingDirectory(workingDirectory);
    process_->start(application, arguments);
}
