#include "data_utils.h"

namespace Data {
QMap <int, float> channelToKmMap;

HighLow floatToHighLow(float& number)
{
    return u32ToHighLow(*reinterpret_cast<quint32*>(&number));
}

HighLow u32ToHighLow(quint32 number)
{
    HighLow val;
    val.low = number;
    val.high = number >> 16;
    return val;
}

void setChannelToKmMap(QMap <int, float>& map) {
    channelToKmMap = map;
}

quint16 writeToBin(quint16 number, int bitNumber, quint16 val)
{
    quint16 index = 1;
    index = index << bitNumber;
    if (val) {
        number = number | index;
    } else {
        number = number & ~index;
    }
    return number;
}


QVector<quint16> eventToVector(QJsonObject& jsonObject)
{
    QVector<quint16> data(7, 0);

    //qDebug() << jsonObject["EventTypeID"].toInt();

    qint64 time = QDateTime::fromString(jsonObject["StartDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000;
    data[0] = (quint16) jsonObject["EventTypeID"].toInt();

    HighLow eventDescriptorID = u32ToHighLow(((quint32)jsonObject["EventDescriptorID"].toInt()));
    data[1] = eventDescriptorID.low;
    data[2] = eventDescriptorID.high;

    HighLow startDateTime = u32ToHighLow((quint32) QDateTime::fromString(jsonObject["StartDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000);
    data[3] = startDateTime.low;
    data[4] = startDateTime.high;

    if (jsonObject["EndDate"].toString().isEmpty()) {
        data[5] = 0;
        data[6] = 0;
    } else {
        HighLow endDateTime = u32ToHighLow((quint32) QDateTime::fromString(jsonObject["EndDate"].toString(), QString("yyyy-MM-ddTHH:mm:ss.000")).toMSecsSinceEpoch()/1000);
        data[5] = endDateTime.low;
        data[6] = endDateTime.high;
    }

    float km = 0.0;
    QMap<int, float>::const_iterator it = channelToKmMap.find(jsonObject["LastChannelNumber"].toInt());
    if (it != channelToKmMap.end()) {
        km = it.value();
    }
    HighLow kmTemp = floatToHighLow(km);
    //data[7] = kmTemp.low;
    //data[8] = kmTemp.high;

    return data;
}
}

