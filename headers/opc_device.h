#pragma once

#include "idevice.h"

#include <QSettings>
#include <QOpcUaProvider>
#include <QOpcUaClient>
#include <QOpcUaErrorState>

class OpcDevice : public IDevice {
    Q_OBJECT
public:
    OpcDevice(QSettings &settingFile);
    virtual ~OpcDevice();

private:
    int initSettings(QSettings &settingFile);
    void saveSettings(QSettings &settingFile);
    void connectDevice();
    QOpcUaProvider *opcUaProvider { nullptr };
    QOpcUaClient *opcUaClient { nullptr };

    bool clConnected { false };
    void createClient();

    void showError(QOpcUaErrorState *errorState);

private slots:
    void onEventData(QJsonArray jsonArray) override;
    // void connectToServer();
    // void findServers();
    void findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode);
    //void getEndpoints();
    void getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);
    void clientConnected();
    void clientDisconnected();
    void namespacesArrayUpdated(const QStringList &namespaceArray);
    //void handleGenericStructHandlerInitFinished(bool success);
    //void handleReadHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult);

};