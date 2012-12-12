#include <QDebug>

#include "WindowsInjector.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

WindowsInjector::WindowsInjector(const QString& libraryPath, QObject* parent):
	Injector(libraryPath, parent),
	process_(NULL) {
}

bool WindowsInjector::startAndAttach(const QString& application, const QStringList& arguments, const QString& workingDirectory) {
	// Clean up old process first
	if (process_ != NULL) {
		delete process_;
	}

    process_ = new QProcess(this);

	attachTimer_ = new QTimer(this);
	attachTimer_->setSingleShot(true);
	attachTimer_->setInterval(500);

    QObject::connect(attachTimer_, SIGNAL(timeout()), this, SLOT(attachToSpawnedProcess()));
    QObject::connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(printStandardOutput()));
    QObject::connect(process_, SIGNAL(readyReadStandardError()), this, SLOT(printStandardError()));
    QObject::connect(process_, SIGNAL(started()), attachTimer_, SLOT(start()));
    QObject::connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), SIGNAL(finished(int)));

    process_->setWorkingDirectory(workingDirectory);
    process_->start(application, arguments);

    return true;
}

void WindowsInjector::attachToSpawnedProcess() {
	if (process_->state() == QProcess::NotRunning) {
		qDebug() << "Failed to start process:" << process_->error();
        return;
    }

    attach(process_->pid());
}

bool WindowsInjector::attach(Q_PID processId) {
	wchar_t path[_MAX_PATH];
	const int pathLength = libraryPath().toWCharArray(path);
	path[pathLength] = 0;

    HMODULE library = ::LoadLibraryW(path);
    if (library == NULL) {
        qDebug() << "Failed to load library: " << libraryPath();
        return false;
    }
    FARPROC hook = ::GetProcAddress(library, "hook_constructor");
    if (hook == NULL) {
        qDebug() << "Failed to load hook_constructor in library";
        return false;
    }

	typedef void(*Installer)(HINSTANCE, DWORD);
	Installer installer = reinterpret_cast<Installer>(hook);
	(*installer)(library, processId->dwThreadId);

    return true;
}

void WindowsInjector::printStandardOutput() {
    qDebug("%s", process_->readAllStandardOutput().data());
}

void WindowsInjector::printStandardError() {
    qCritical("%s", process_->readAllStandardError().data());
}
