#pragma once

#include <QObject>
#include <QJsonArray>

class IDevice : public QObject
{
    Q_OBJECT

public:
    IDevice() {}
    virtual ~IDevice() {}

public slots:
    virtual void onEventData(QJsonArray jsonArray) = 0;
};