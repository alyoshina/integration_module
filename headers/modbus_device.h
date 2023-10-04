#pragma once

#include "idevice.h"

#include <QObject>
#include <QModbusClient>
#include <QSettings>
#include <QJsonArray>
#include <QModbusDataUnit>
#include <QJsonArray>
#include <QJsonObject>

typedef struct
{
    unsigned int START_ADDRESS = 1127;
    unsigned int AMOUNT = 9;
    unsigned int WRITE_SIZE = 1;
    int serverAddress = 1;
    int responseTime = 100;
    int numberOfRetries = 10;
    QString port = "2000";
    QString ip = "127.0.0.1";
} ModbusSettings;

class ModbusDevice : public IDevice
{
    Q_OBJECT

public:
    ModbusDevice(QSettings &settingFile);
    virtual ~ModbusDevice();

    void connectDevice();
    QModbusClient *device;
    QString getStateStr(int state);

private:
    ModbusSettings modbusSettings;
    int initSettings(QSettings &settingFile);
    void saveSettings(QSettings &settingFile);
    void setEventData(QJsonArray& jsonArray);
    void writeDataUnit(QModbusDataUnit& unit);

private slots:
    void onEventData(QJsonArray jsonArray) override;
    void onStateChanged(int state);

public slots:
    void write(QJsonArray jsonArray);
};
