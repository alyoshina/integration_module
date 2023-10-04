#include "modbus_device.h"
#include "data_utils.h"

#include <QModbusTcpClient>
#include <QDebug>

ModbusDevice::ModbusDevice(QSettings &settingFile):
    device(nullptr)
{
    initSettings(settingFile);
    saveSettings(settingFile);

    connectDevice();
}

ModbusDevice::~ModbusDevice()
{
    if (device) {
        device->disconnectDevice();
    }
    delete device;
}

int ModbusDevice::initSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Modbus");
    modbusSettings.port = settingFile.value("port", "502").toString();
    modbusSettings.ip = settingFile.value("ip", "127.0.0.1").toString();
    modbusSettings.responseTime = settingFile.value("responseTime", "100").toInt();
    modbusSettings.numberOfRetries = settingFile.value("numberOfRetries", "5").toInt();
    modbusSettings.serverAddress = settingFile.value("serverAddress", "1").toInt();
    settingFile.endGroup();
    settingFile.sync();

    qDebug() << "Settings for ModbusDevice loaded";

    return 1;
}

void ModbusDevice::saveSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Modbus");
    settingFile.setValue("port", modbusSettings.port);
    settingFile.setValue("ip", modbusSettings.ip);
    settingFile.setValue("responseTime", modbusSettings.responseTime);
    settingFile.setValue("numberOfRetries", modbusSettings.numberOfRetries);
    settingFile.setValue("serverAddress", modbusSettings.serverAddress);
    settingFile.endGroup();
    settingFile.sync();

    qDebug() << "Settings for ModbusDevice save";
}


void ModbusDevice::connectDevice()
{
    if (device) {
        device->disconnectDevice();
        delete device;
        device = nullptr;
    }

    //TODO add rtu
    device = new QModbusTcpClient();

    connect(device, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << device->errorString();
    });
    connect(device, &QModbusClient::stateChanged, this, &ModbusDevice::onStateChanged);

    if (device) {
        device->setConnectionParameter(QModbusDevice::NetworkPortParameter, modbusSettings.port.toInt());
        device->setConnectionParameter(QModbusDevice::NetworkAddressParameter, modbusSettings.ip);
        device->setTimeout(modbusSettings.responseTime);
        device->setNumberOfRetries(modbusSettings.numberOfRetries);
        if (!device->connectDevice()) {
            qDebug() << "Modbus connect failed: " << device->errorString();
        } else {
            qDebug() << getStateStr(device->state());
        }
    }
}

void ModbusDevice::onStateChanged(int state)
{
    //bool connected = (state != QModbusDevice::UnconnectedState);
    qDebug() << getStateStr(state);
}

QString ModbusDevice::getStateStr(int state)
{
    QString srt("Modbus state is unknown");
    switch (state) {
    case QModbusDevice::UnconnectedState:
        srt = "Modbus state is unconnected";
        break;
    case QModbusDevice::ConnectingState:
        srt = "Modbus state is connecting";
        break;
    case QModbusDevice::ConnectedState:
        srt = "Modbus state is connected";
        break;
    case QModbusDevice::ClosingState:
        srt = "Modbus state is closing";
        break;
    default:
        ;
        break;
    }
    return srt;
}

void ModbusDevice::write(QJsonArray jsonArray)
{
    if (!device) {
        return;
    }
    qDebug() << "ModbusDevice write" << jsonArray.size();
    //QModbusDataUnit writeUnit = writeRequest();
}

void ModbusDevice::writeDataUnit(QModbusDataUnit& unit)
{
    if (!device) {
        return;
    }
    if (auto *reply = device->sendWriteRequest(unit, modbusSettings.serverAddress)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << QString("Write response error: %1 (Mobus exception: 0x%2)")
                                .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                } else if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << QString("Write response error: %1 (code: 0x%2)")
                                .arg(reply->errorString()).arg(reply->error(), -1, 16);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        qDebug() << QString("Write error: ") << device->errorString();
    }
    qDebug() << "ModbusDevice writeDataUnit" ;
}

void ModbusDevice::onEventData(QJsonArray jsonArray)
{
    setEventData(jsonArray);
}

void ModbusDevice::setEventData(QJsonArray& jsonArray)
{
    int address = 0;
    QVector<quint16> data;

    quint16 reg1 = Data::writeToBin(0, 1, 1);
    reg1 = Data::writeToBin(reg1, 4, 1);
    reg1 = Data::writeToBin(reg1, 5, 0);
    reg1 = Data::writeToBin(reg1, 0, 0);

    quint16 reg2 = Data::writeToBin(0, 1, 0);

    QString str1;
    str1.setNum(reg1, 16);
    QString str2;
    str2.setNum(reg2, 16);

    data.append(reg1);
    data.append(reg2);
    QJsonObject obj = jsonArray[jsonArray.size()-1].toObject();
    data.append(Data::eventToVector(obj));

    QModbusDataUnit unit(QModbusDataUnit::RegisterType::HoldingRegisters, address, data);
    writeDataUnit(unit);
}

