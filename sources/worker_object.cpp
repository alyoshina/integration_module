#include "worker_object.h"
#include "modbus_device.h"
#include "opc_device.h"
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
    initSettings(settingFile);
    saveSettings(settingFile);

    //TODO device selection
    switch (deviceType)
    {
    case DeviceType::Modbus:
        device = new ModbusDevice(settingFile);
        break;
    case DeviceType::Opc:
        device = new OpcDevice(settingFile);
        break;
    }

    if (device) {
        connect(eventObject, SIGNAL(eventData(QJsonArray)), device, SLOT(onEventData(QJsonArray)));
    } else {
        qCritical() << "Device not selected";
    }
    
}

WorkerObject::~WorkerObject() {
    if (eventObject) {
        emit eventObject->finished();
    }
    delete device;
}

DeviceType WorkerObject::strToType(QString& str) {
    if (str == QString("modbus")) {
        return DeviceType::Modbus;
    }
    if (str == QString("opc")) {
        return DeviceType::Opc;
    }
    return deviceType;
}

QString WorkerObject::typeToStr(DeviceType type) {
    QString str("modbus");
    switch (type) {
    case DeviceType::Modbus:
        str = QString("modbus");
        break;
    case DeviceType::Opc:
        str = QString("opc");
        break;
    }
    return str;
}

int WorkerObject::initSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    QString type = settingFile.value("deviceType", "modbus").toString();
    deviceType = strToType(type);
    settingFile.endGroup();
    settingFile.sync();

    return 1;
}

void WorkerObject::saveSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    settingFile.setValue("deviceType", typeToStr(deviceType));
    settingFile.endGroup();
    settingFile.sync();
}

void WorkerObject::onChannelToKm(QMap <int, float> map)
{
    Data::setChannelToKmMap(map);
}
