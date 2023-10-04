#include "db_service.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QJsonObject>
#include <QSqlRecord>

DbService::DbService(QSettings &settingFile)
{
    initSettings(settingFile);
    saveSettings(settingFile);
}

DbService::~DbService()
{
    qDebug() << "delete DbService";
}

bool DbService::createConnectionToDb()
{
    QStringList drivers = QSqlDatabase::drivers();
    foreach (QString driver, drivers) {
        qCritical() << "driver = " << driver.toUtf8().constData();
        qWarning() << "\t"<<driver.toUtf8().constData();
    }

    if (!QSqlDatabase::drivers().contains("QPSQL")) {
        qWarning() << "Unable to load database. Needs the QPSQL driver.";
        return false;
    }

    for (int i = 0; i < 2; i++) {
        QString name_ = QString("qtConnToDb")+QString::number(i);
        QSqlDatabase connToDb_ = QSqlDatabase::addDatabase("QPSQL", name_);

        connToDb_.setHostName(dbSettings.dbHost);
        connToDb_.setPort(QString(dbSettings.dbPort).toInt());
        connToDb_.setDatabaseName(dbSettings.dbName);
        connToDb_.setUserName(dbSettings.dbLogin);
        connToDb_.setPassword(dbSettings.dbPassword);

        if (!connToDb_.open()) {
            qCritical() << "QSqlDatabase not connected name=" << name_;
            qCritical() << connToDb_.lastError().text().toUtf8().constData();
            return false;
        } else {
            qCritical() << "QSqlDatabase connected name=" << name_;
            connNameToDbVector.push_back(name_);
        }
    }

    return true;
}

int DbService::initSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    dbSettings.dbName = settingFile.value("dbName", "Sit").toString();
    dbSettings.dbHost = settingFile.value("dbHost", "127.0.0.1").toString();
    dbSettings.dbPort = settingFile.value("dbPort", "5432").toString();
    dbSettings.dbLogin = settingFile.value("dbLogin", "").toString();
    dbSettings.dbPassword = settingFile.value("dbPassword", "").toString();
    settingFile.endGroup();
    settingFile.sync();

    qDebug() << "Settings for db loaded";

    return 1;
}

void DbService::saveSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Settings");
    settingFile.setValue("dbName", dbSettings.dbName);
    settingFile.setValue("dbHost", dbSettings.dbHost);
    settingFile.setValue("dbPort", dbSettings.dbPort);
    settingFile.setValue("dbLogin", dbSettings.dbLogin);
    settingFile.setValue("dbPassword", dbSettings.dbPassword);
    settingFile.endGroup();
    settingFile.sync();

    qDebug() << "Settings for db save";
}

QString DbService::getConnNameToDb()
{
    connNameToDbMutex.lock();
    while (connNameToDbVector.empty()) {
        waitCondition.wait(&connNameToDbMutex);
    }
    QString qtConnName = connNameToDbVector.front();
    connNameToDbVector.pop_front();
    connNameToDbMutex.unlock();

    return qtConnName;
}

void DbService::putConnNameToDb(QString qtConnName)
{
    connNameToDbMutex.lock();
    connNameToDbVector.push_back(qtConnName);
    waitCondition.wakeAll();
    connNameToDbMutex.unlock();
}

QMap <int, float> DbService::exec(QString sql, QString type)
{
    QMap <int, float> map;

    QString connNameToDb = getConnNameToDb();
    QSqlDatabase conn = QSqlDatabase::database(connNameToDb);
    QSqlQuery query(conn);
    query.exec(sql);
    putConnNameToDb(connNameToDb);

    if (!query.isActive()) {
        qCritical() << query.lastError().text().toUtf8().constData();
    } else {
        if (query.size()>0) {
            while (query.next()) {
                map.insert(query.value(0).toInt(), query.value(1).toFloat());
            }
        }
    }

    return map;
}

QJsonArray DbService::exec(QString sql)
{
    QJsonArray recordsArray;

    QString connNameToDb = getConnNameToDb();
    QSqlDatabase conn = QSqlDatabase::database(connNameToDb);
    QSqlQuery query(conn);
    query.exec(sql);
    putConnNameToDb(connNameToDb);

    if (!query.isActive()) {
        qCritical() << query.lastError().text().toUtf8().constData();
    } else {
        if (query.size()>0) {
            while (query.next()) {
                QJsonObject recordObject;
                for(int i = 0; i < query.record().count(); i++) {

                    recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
                }
                recordsArray.push_back(recordObject);
            }
        }
    }
    return recordsArray;
}