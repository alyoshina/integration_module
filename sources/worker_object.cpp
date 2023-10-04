#include "worker_object.h"
#include "modbus_device.h"
#include "data_utils.h"

#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>

WorkerObject::WorkerObject() : QObject()
{
    eventObject = new EventObject();
    QThread *thread = new QThread;
    eventObject->moveToThread(thread);
    connect(thread, SIGNAL(started()), eventObject, SLOT(process()));
    connect(eventObject, SIGNAL(finished()), thread, SLOT(quit()));
    connect(eventObject, SIGNAL(finished()), eventObject, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(eventObject, SIGNAL(channelToKm(QMap <int, float>)), this, SLOT(onChannelToKm(QMap <int, float>)));
    thread->start();

    QSettings settingFile("config.ini", QSettings::IniFormat);
    device = new ModbusDevice(settingFile);

    //TODO device selection
    /*
    DeviceType type;
    switch (type)
    {
    case DeviceType::Modbus:
        device = new ModbusDevice(settingFile);
        break;
    case DeviceType::Opc:
        //device = new OpcDevice(settingFile);
        break;
    }
    */

    if (device) {
        connect(eventObject, SIGNAL(eventData(QJsonArray)), device, SLOT(onEventData(QJsonArray)));
    }
    
}

WorkerObject::~WorkerObject() {
    if (eventObject) {
        emit eventObject->finished();
    }
    delete device;
}


void WorkerObject::onChannelToKm(QMap <int, float> map)
{
    Data::setChannelToKmMap(map);
}
