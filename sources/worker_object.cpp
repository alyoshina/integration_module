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

    //TODO device selection
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

// HighLow WorkerObject::floatToHighLow(float number)
// {
//     union
//     {
//         quint32 i;
//         float f;
//     } u;

//     u.f = number;
//     return u32ToHighLow(u.i);
// }

// HighLow WorkerObject::u32ToHighLow(quint32 number)
// {
//     HighLow val;
//     val.low = number;
//     val.high = number >> 16;
//     return val;
// }

// quint16 WorkerObject::writeToBin(quint16 number, int bitNumber, quint16 val)
// {
//     quint16 index = 1;
//     index = index << bitNumber;
//     if (val) {
//         number = number | index;
//     } else {
//         number = number & ~index;
//     }
//     return number;
// }


// QVector<quint16> WorkerObject::eventToVector(QJsonObject jsonObject)
// {
// //QJsonObject({"Comment":"","DateConfirmed":null,"Description":"test","EndDate":"2022-11-02T11:00:52.000","EventDescriptorID":1761031,"EventTypeID":11,"LastActiveStatus":1,"LastChannelNumber":10000,"LastEventTime":"2022-11-02T11:00:52.000","LastLatitude":0,"LastLongtitude":0,"LastSpeed":0,"LastWidth":2,"ObjectTypeID":1,"SensorID":-1,"StartChannelNumber":10000,"StartDate":"2022-11-02T11:00:52.000","isConfirmed":-1}))
//     QVector<quint16> data(7, 0);

//     qDebug() << jsonObject["EventTypeID"].toInt();

//     qDebug() << jsonObject["EventDescriptorID"].toInt();
//     qint64 time = QDateTime::fromString(jsonObject["StartDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000;
//     //qint64 timeISODate = QDateTime::fromString(jsonObject["StartDate"].toString(), Qt::ISODate).toMSecsSinceEpoch();
//     qDebug() << time; //1667386852 1667376052000 timeZone


//     //qDebug() << QDateTime::fromString(jsonObject["StartDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).timeZone();
//     qDebug() << jsonObject["StartDate"];
//     qDebug() << jsonObject["EndDate"];
//     qDebug() << jsonObject["LastChannelNumber"].toInt();


//     //------
//     data[0] = (quint16) jsonObject["EventTypeID"].toInt();

//     HighLow eventDescriptorID = u32ToHighLow(((quint32)jsonObject["EventDescriptorID"].toInt()));
//     data[1] = eventDescriptorID.low;
//     data[2] = eventDescriptorID.high;

//     HighLow startDateTime = u32ToHighLow((quint32) QDateTime::fromString(jsonObject["StartDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000);
//     data[3] = startDateTime.low;
//     data[4] = startDateTime.high;

//     if (jsonObject["EndDate"].toString().isEmpty()) {
//         data[5] = 0;
//         data[6] = 0;
//     } else {
//         HighLow endDateTime = u32ToHighLow((quint32) QDateTime::fromString(jsonObject["EndDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000);
//         data[5] = endDateTime.low;
//         data[6] = endDateTime.high;
//     }

//     float km = 0.0;
//     QMap<int, float>::const_iterator it = channelToKmMap.find(jsonObject["LastChannelNumber"].toInt());
//     if (it != channelToKmMap.end()) {
//         km = it.value();
//     }
//     HighLow kmTemp = floatToHighLow(km);
//     //data[7] = kmTemp.low;
//     //data[8] = kmTemp.high;

//     return data;
// }

// void WorkerObject::onEventData(QJsonArray jsonArray)
// {
//     qDebug() << "WorkerObject" << jsonArray.size();
//     //eventToVector(jsonArray[jsonArray.size()-1].toObject());
//     qDebug() << "WorkerObject" << jsonArray[jsonArray.size()-1];
// //    quint32 v = 945168415;
// //    HighLow val = u32ToHighLow(v);
// //    qDebug() << val.low; //14422
// //    qDebug() << val.high; //8223

// //    qDebug() << bool(writeToBin(510, 0, 1) == 511);
// //    qDebug() << bool(writeToBin(510, 0, 0) == 510);
// //    qDebug() << bool(writeToBin(510, 7, 0) == 382);
// //    qDebug() << bool(writeToBin(382, 7, 1) == 510);
// //    qDebug() << bool(writeToBin(32768, 15, 0) == 0);
// //    qDebug() << bool(writeToBin(32768, 14, 1) == 49152);

//     int address = 0;
//     QVector<quint16> data;

//     quint16 reg1 = writeToBin(0, 1, 1);
//     reg1 = writeToBin(reg1, 4, 1);
//     reg1 = writeToBin(reg1, 5, 0);
//     reg1 = writeToBin(reg1, 0, 0);

//     quint16 reg2 = writeToBin(0, 1, 0);

//     qDebug() << reg1 << reg2;
//     QString str1;
//     str1.setNum(reg1, 16);
//     QString str2;
//     str2.setNum(reg2, 16);
//     qDebug() << str1 << str2;

//     //---------
//     //---
//     data.append(reg1);
//     data.append(reg2);

//     data.append(eventToVector(jsonArray[jsonArray.size()-1].toObject()));

//     //---

//     QModbusDataUnit unit(QModbusDataUnit::RegisterType::HoldingRegisters, address, data);
//     emit eventData(unit);

//     //QModbusDataUnit(table, startAddress, numberOfEntries);

// }
