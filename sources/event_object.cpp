#include "event_object.h"

#include <QDebug>
#include <QTest>

EventObject::EventObject() :
    routeID(5)
{
    QSettings settingFile("config.ini", QSettings::IniFormat);
    initSettings(settingFile);
    saveSettings(settingFile);
    dbService = new DbService(settingFile);
}

EventObject::~EventObject()
{
    delete dbService;
}

int EventObject::initSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    routeID = settingFile.value("routeID", "5").toInt();
    settingFile.endGroup();
    settingFile.sync();

    return 1;
}

void EventObject::saveSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    settingFile.setValue("routeID", routeID);
    settingFile.endGroup();
    settingFile.sync();
}

void EventObject::process()
{
    dbService->createConnectionToDb();
    //channelTokm
    {
        QString query = QString("select \"ChannelNumber\", \"Km\" from public.\"tVirtualRoutes\" where \"RouteID\"=%1").arg(routeID);
        QMap <int, float> channelTokmMap = dbService->exec(query, QString("map"));
        emit channelToKm(channelTokmMap);
        QTest::qSleep(10000);
    }


    while(1) {
        qDebug() << "EventObject::process";
        QString query = QString("select * from sp_ngeteventsautowriter(%1, 0, 0)").arg(routeID);
        QJsonArray jsonArray = dbService->exec(query);

        emit eventData(jsonArray);
        QTest::qSleep(10000);
    }
    emit finished();
    return;
}

void EventObject::stop()
{
    return;
}
