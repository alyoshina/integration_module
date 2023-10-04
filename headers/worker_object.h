#pragma once

#include "idevice.h"
#include "event_object.h"

#include <QObject>
#include <QMap>

class WorkerObject : public QObject
{
    Q_OBJECT
public:
    WorkerObject();
    virtual ~WorkerObject();

private:
    EventObject *eventObject { nullptr };
    IDevice *device { nullptr };
    enum class DeviceType { Modbus, Opc };

public slots:
    void onChannelToKm(QMap <int, float> map);

};

