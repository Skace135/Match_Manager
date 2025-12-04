#include "engineProcess.h"
#include <iostream>
#include "qDebug.h"

EngineProcess::EngineProcess(QObject* parent)
    : QObject(parent), process_(new QProcess(this))
{
    connect(process_, &QProcess::readyReadStandardOutput,
            this, &EngineProcess::handleReadyRead);

}

void EngineProcess::start(const QString& path)
{
    process_->start(path);
}

void EngineProcess::send(const QString& command)
{
    if(process_->state() == QProcess::Running) {
        process_->write((command + "\n").toUtf8());
    }
}

void EngineProcess::handleReadyRead()
{
    while(process_->canReadLine()) {
        QByteArray line = process_->readLine();
        emit outputReady(QString::fromUtf8(line).trimmed());
    }
}
