#pragma once

#include <QVector>
#include <QMap>
#include <QModbusDataUnit>
#include <QJsonArray>
#include <QJsonObject>

namespace Data {
    
    union {
        quint32 i;
        float f;
    } Union32float;

    struct HighLow {
        quint16 high;
        quint16 low;
    };

    HighLow floatToHighLow(float& number);
    HighLow u32ToHighLow(quint32 number);
    quint16 writeToBin(quint16 number, int bitNumber, quint16 val);
    QVector<quint16> eventToVector(QJsonObject& jsonObject);
    void setChannelToKmMap(QMap <int, float>& map);
}