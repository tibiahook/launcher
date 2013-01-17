#include "Initializer.h"
#include "Injector.h"

Initializer::Initializer(const QString& libraryPath, const QString& tibiaPath, QObject* parent):
    QObject(parent),
    libraryPath_(libraryPath),
    tibiaPath_(tibiaPath),
    injector_(new Injector(libraryPath_, this)) {

    // Connect signal to signal
    QObject::connect(injector_, &Injector::finished, this, &Initializer::finished);
}

Initializer::~Initializer() {
    delete injector_;
}

void Initializer::start() {
    injector_->startAndAttach(tibiaPath_, QStringList());
}
