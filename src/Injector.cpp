#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QMutableListIterator>
#include <QVariant>

#include "Injector.h"

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

bool Injector::startAndAttach(const QString& application, const QStringList& arguments) {
    // Clean up old process first
    if (process_ != NULL) {
        delete process_;
    }

    // Working directory is same as Tibia's directory
    QString workingDirectory = QFileInfo(application).canonicalPath();

    // Copy config.js file to the Tibia directory
    QDir configDirectory(QFileInfo(libraryPath_).absoluteDir());
    if (!rewriteConfig(configDirectory.absoluteFilePath("config.json"), QDir(workingDirectory).absoluteFilePath("config.json"))) {
        qDebug() << "Failed to rewrite config.json!";
        return false;
    }

    process_ = new QProcess(this);
    process_->setWorkingDirectory(workingDirectory);

#ifndef Q_OS_WIN
    // Generate library list
    QStringList libraries;
    libraries << "/home/gillis/qt5.0.0/5.0.0/gcc/lib/libQt5Core.so";
    libraries << "/home/gillis/qt5.0.0/5.0.0/gcc/lib/libQt5Gui.so";
    libraries << "/home/gillis/qt5.0.0/5.0.0/gcc/lib/libQt5Widgets.so";
    libraries << libraryPath_;

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("LD_PRELOAD", libraries.join(":"));

    // Set LD_PRELOAD so it loads the Qt libraries
    process_->setProcessEnvironment(environment);
#endif

    typedef void (QProcess::*OverloadedFinished)(int, QProcess::ExitStatus);

    QObject::connect(process_, &QProcess::started, this, &Injector::attachToSpawnedProcess);
    QObject::connect(process_, &QProcess::readyReadStandardOutput, this, &Injector::printStandardOutput);
    QObject::connect(process_, &QProcess::readyReadStandardError, this, &Injector::printStandardError);
    QObject::connect(process_, (OverloadedFinished) &QProcess::finished, this, &Injector::finished);

    // Start Tibia
    process_->start(application, arguments);

    return true;
}

void Injector::attachToSpawnedProcess() {
#ifdef Q_OS_WIN
    Q_PID processId = process_->pid();

    // Write the path to a C string
    int length = libraryPath_.length() + 1;
    char path[length];
    memcpy(path, libraryPath_.toStdString().c_str(), length);

    // Find the LoadLibrary function and write the library path in the external process
    LPVOID loadLibrary = (LPVOID) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    LPVOID externalPath = (LPVOID) VirtualAllocEx(processId->hProcess, NULL, length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(processId->hProcess, (LPVOID) externalPath, path, length, NULL);

    // Run LoadLibraryA in the Tibia thread and wait for it to finish
    HANDLE thread = CreateRemoteThread(processId->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) loadLibrary, (LPVOID) externalPath, 0, NULL);
    WaitForSingleObject(thread, INFINITE);

    // Clean up the thread and the external memory
    CloseHandle(thread);
    VirtualFreeEx(processId->hProcess, (LPVOID) externalPath, 0, MEM_RELEASE);
#endif
}

bool Injector::rewriteConfig(const QString& source, const QString& destination) {
    QFile sourceFile(source);
    if (!sourceFile.open(QFile::ReadOnly)) {
        qDebug() << "Could not open config file " << source << "!";
        return false;
    }

    QJsonDocument document(QJsonDocument::fromJson(sourceFile.readAll()));
    if (document.isNull()) {
        qDebug() << "Could not parse config file " << source << "!";
        return false;
    }

    QVariant settings = document.toVariant();
    if (settings.type() != QVariant::Map) {
        qDebug() << "Invalid config file " << source << "!";
        return false;
    }

    QVariant pluginsResult = settings.toMap().value("plugins");
    if (pluginsResult.type() != QVariant::List) {
        qDebug() << "Invalid plugins list!";
        return false;
    }

    QVariantList plugins = pluginsResult.toList();
    QDir sourceDirectory(QFileInfo(source).absoluteDir());

    QMutableListIterator<QVariant> iterator(plugins);
    while (iterator.hasNext()) {
        QVariant& plugin = iterator.next();
        if (plugin.type() != QVariant::String) {
            qDebug() << "Invalid plugin directory " << plugin << "!";
            return false;
        }
        QString pluginPath(plugin.toString());
        if (!QFileInfo(pluginPath).isAbsolute()) {
            QString pluginAbsolutePath = QDir::cleanPath(sourceDirectory.absoluteFilePath(pluginPath));
            iterator.setValue(pluginAbsolutePath);
        }
    }

    // Create new settings file
    QVariantMap newSettings = settings.toMap();
    newSettings["plugins"] = plugins;

    // Create new config file
    QFile destinationFile(destination);
    if (!destinationFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Could create destination config file!";
        return false;
    }

    destinationFile.write(QJsonDocument::fromVariant(newSettings).toJson());
    destinationFile.close();

    return true;
}

void Injector::printStandardOutput() {
    qDebug("%s", process_->readAllStandardOutput().data());
}

void Injector::printStandardError() {
    qCritical("%s", process_->readAllStandardError().data());
}
