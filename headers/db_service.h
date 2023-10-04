#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QMutex>
#include <QWaitCondition>
#include <QJsonArray>

typedef struct
{
    QString dbHost;
    QString dbPort;
    QString dbName;
    QString dbLogin;
    QString dbPassword;
}DbSettings;

class DbService : public QObject
{
   Q_OBJECT
public:
    DbService(QSettings &settingFile);
    virtual ~DbService();

    QJsonArray exec(QString sql);
    QMap <int, float> exec(QString sql, QString type);
    bool createConnectionToDb();

private:
    DbSettings dbSettings;
    QList <QString> connNameToDbVector;
    int initSettings(QSettings &settingFile);
    void saveSettings(QSettings &settingFile);
    QString getConnNameToDb();
    void putConnNameToDb(QString qtConnName);

    QMutex connNameToDbMutex;
    QWaitCondition waitCondition;
};
