#pragma once

#include "idevice.h"
#include "event_object.h"

#include <QObject>
#include <QMap>

enum class DeviceType : int { Modbus, Opc };

class WorkerObject : public QObject
{
    Q_OBJECT
public:
    WorkerObject();
    virtual ~WorkerObject();

private:
    EventObject *eventObject { nullptr };
    IDevice *device { nullptr };
    DeviceType deviceType { DeviceType::Modbus };
    int initSettings(QSettings &settingFile);
    void saveSettings(QSettings &settingFile);
    DeviceType strToType(QString& str);
    QString typeToStr(DeviceType type);

public slots:
    void onChannelToKm(QMap <int, float> map);
};

