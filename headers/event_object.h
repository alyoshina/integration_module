#pragma once

#include "db_service.h"

#include <QObject>
#include <QThread>
#include <QSettings>


class EventObject : public QObject
{
    Q_OBJECT
public:
    EventObject();
    virtual ~EventObject();

private:
    int initSettings(QSettings &settingFile);
    void saveSettings(QSettings &settingFile);
    DbService *dbService;
    int routeID;

public slots:
    void process();
    void stop();

signals:
    void finished();
    void eventData(QJsonArray jsonArray);
    void channelToKm(QMap <int, float> map);
};

