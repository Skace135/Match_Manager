// engineProcess.h
#pragma once
#include <QProcess>
#include <QObject>

class EngineProcess : public QObject
{
    Q_OBJECT
public:
    EngineProcess(QObject* parent = nullptr);

    void start(const QString& path);
    void send(const QString& command);

signals:
    void outputReady(const QString& line);

private slots:
    void handleReadyRead();

private:
    QProcess* process_;
};
