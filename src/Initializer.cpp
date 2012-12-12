#include "Initializer.h"
#include "Injector.h"

#ifdef Q_OS_WIN
#include "WindowsInjector.h"
#define PlatformInjector WindowsInjector
#else
#include "UnixInjector.h"
#define PlatformInjector UnixInjector
#endif

Initializer::Initializer(const QString& libraryPath, const QString& tibiaPath, const QString& workingDirectory, QObject* parent):
	QObject(parent),
	libraryPath_(libraryPath),
	tibiaPath_(tibiaPath),
    workingDirectory_(workingDirectory) {
}

void Initializer::run() {
	Injector* injector = new PlatformInjector(libraryPath_);

    connect(injector, SIGNAL(finished(int)), SIGNAL(finished()));
    injector->startAndAttach(tibiaPath_, QStringList(), workingDirectory_);
}
