#ifndef INJECTOR_H
#define INJECTOR_H

#include <QObject>
#include <QProcess>
#include <QString>

class Injector: public QObject {
    Q_OBJECT

public:
    Injector(const QString& libraryPath, QObject* parent):
        QObject(parent),
        libraryPath_(libraryPath),
        process_(NULL) {}

    bool startAndAttach(const QString& application, const QStringList& arguments);

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private slots:
    void printStandardOutput();
    void printStandardError();

    void attachToSpawnedProcess();

private:
    bool rewriteConfig(const QString& source, const QString& destination);

    QString libraryPath_;
    QProcess* process_;
};

#endif
