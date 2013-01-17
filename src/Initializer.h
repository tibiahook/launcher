#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <QObject>
#include <QProcess>
#include <QString>

class Injector;

class Initializer: public QObject {
    Q_OBJECT

public:
    Initializer(const QString& libraryPath, const QString& tibiaPath, QObject* parent = 0);
    ~Initializer();

public slots:
    void start();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QString libraryPath_;
    QString tibiaPath_;

    Injector* injector_;
};

#endif
