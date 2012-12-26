#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutableListIterator>
#include <QVariant>

#include <json.h>

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

    // Copy config.js file to the Tibia directory
    QDir configDirectory(QFileInfo(libraryPath()).absoluteDir());
    QDir workingDir(workingDirectory);
    copyAdjustedPluginSettings(configDirectory.absoluteFilePath("config.json"), workingDir.absoluteFilePath("config.json"));

    // Generate library list
    QStringList libraries;
    libraries << "/usr/lib/i386-linux-gnu/libQtCore.so";
    libraries << "/usr/lib/i386-linux-gnu/libQtGui.so";
    libraries << libraryPath_;

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("LD_PRELOAD", libraries.join(":"));

    process_ = new QProcess(this);

    QObject::connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(printStandardOutput()));
    QObject::connect(process_, SIGNAL(readyReadStandardError()), this, SLOT(printStandardError()));
    QObject::connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), SIGNAL(finished(int)));

    process_->setProcessEnvironment(environment);
    process_->setWorkingDirectory(workingDirectory);
    process_->start(application, arguments);

    return process_->waitForStarted();
}

bool UnixInjector::copyAdjustedPluginSettings(const QString& source, const QString& destination) {
    QFile sourceFile(source);
    if (!sourceFile.open(QFile::ReadOnly)) {
        qDebug() << "Could not open config file!";
        return false;
    }

    bool parsed = false;
    QVariant settings = QtJson::parse(sourceFile.readAll(), parsed);
    if (!parsed || settings.type() != QVariant::Map) {
        qDebug() << "Invalid config file!";
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
            qDebug() << "Invalid plugin " << plugin << "!";
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

    destinationFile.write(QtJson::serialize(newSettings));
    destinationFile.close();

    return true;
}

void UnixInjector::printStandardOutput() {
    qDebug("%s", process_->readAllStandardOutput().data());
}

void UnixInjector::printStandardError() {
    qCritical("%s", process_->readAllStandardError().data());
}
