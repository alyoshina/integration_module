#include "opc_device.h"

OpcDevice::OpcDevice(QSettings &settingFile)
{
    initSettings(settingFile);
    saveSettings(settingFile);

    connectDevice();
}

OpcDevice::~OpcDevice()
{
    //TODO
    //opcUaProvider
    //opcUaClient
}

int OpcDevice::initSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Opc");

    settingFile.endGroup();
    settingFile.sync();

    return 1;
}

void OpcDevice::saveSettings(QSettings &settingFile)
{
    settingFile.beginGroup("Opc");

    settingFile.endGroup();
    settingFile.sync();
}

void OpcDevice::connectDevice()
{
    if (clConnected) {
        opcUaClient->disconnectFromEndpoint();
        return;
    }
    // if (device) {
    //     device->disconnectDevice();
    //     delete device;
    //     device = nullptr;
    // }
    //disconnect(opcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcDevice::namespacesArrayUpdated);
}

void OpcDevice::showError(QOpcUaErrorState *errorState)
{
    //int result = 0;
    const QString statuscode = QOpcUa::statusToString(errorState->errorCode());
    QString msg = errorState->isClientSideError() ? tr("The client reported: ") : tr("The server reported: ");

    switch (errorState->connectionStep()) {
    case QOpcUaErrorState::ConnectionStep::Unknown:
        break;
    case QOpcUaErrorState::ConnectionStep::CertificateValidation: {
        //CertificateDialog dlg(this);
        msg += tr("Server certificate validation failed with error 0x%1 (%2).\nClick 'Abort' to abort the connect, or 'Ignore' to continue connecting.")
                  .arg(static_cast<ulong>(errorState->errorCode()), 8, 16, QLatin1Char('0')).arg(statuscode);
        //result = dlg.showCertificate(msg, m_endpoint.serverCertificate(), m_pkiConfig.trustListDirectory());
        //errorState->setIgnoreError(result == 1);
    }
        break;
    case QOpcUaErrorState::ConnectionStep::OpenSecureChannel:
        msg += tr("OpenSecureChannel failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        qCritical() << "Connection Error:" << msg;
        break;
    case QOpcUaErrorState::ConnectionStep::CreateSession:
        msg += tr("CreateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        qCritical() << "Connection Error:" << msg;
        break;
    case QOpcUaErrorState::ConnectionStep::ActivateSession:
        msg += tr("ActivateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        qCritical() << "Connection Error:" << msg;
        break;
    }
}

void OpcDevice::createClient()
{
    if (!opcUaClient && opcUaProvider) {
        opcUaClient = opcUaProvider->createClient("text");//(ui->opcUaPlugin->currentText());
        if (!opcUaClient) {
            qCritical() << "Connecting to the given opc sever failed. Failed to connect to server";
            return;
        }

        connect(opcUaClient, &QOpcUaClient::connectError, [this](QOpcUaErrorState *errorState) {
            showError(errorState);
        });
        // opcUaClient->setApplicationIdentity(m_identity);
        // opcUaClient->setPkiConfiguration(m_pkiConfig);

        // if (opcUaClient->supportedUserTokenTypes().contains(QOpcUaUserTokenPolicy::TokenType::Certificate)) {
        //     QOpcUaAuthenticationInformation authInfo;
        //     authInfo.setCertificateAuthentication();
        //     opcUaClient->setAuthenticationInformation(authInfo);
        // }

        connect(opcUaClient, &QOpcUaClient::connected, this, &OpcDevice::clientConnected);
        connect(opcUaClient, &QOpcUaClient::disconnected, this, &OpcDevice::clientDisconnected);
        connect(opcUaClient, &QOpcUaClient::errorChanged, [this](QOpcUaClient::ClientError error) {
            qCritical() << "Client error changed" << error;
        });
        connect(opcUaClient, &QOpcUaClient::stateChanged, [this](QOpcUaClient::ClientState state) {
            qCritical() << "Client state changed" << state;
        });
        connect(opcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &OpcDevice::getEndpointsComplete);
        connect(opcUaClient, &QOpcUaClient::findServersFinished, this, &OpcDevice::findServersComplete);
    }
}

void OpcDevice::clientConnected()
{
    clConnected = true;
    //updateState();

    connect(opcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcDevice::namespacesArrayUpdated);
    opcUaClient->updateNamespaceArray();
}

void OpcDevice::clientDisconnected()
{
    clConnected = false;
    opcUaClient->deleteLater();
    opcUaClient = nullptr;
    //updateState();
}

void OpcDevice::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    // if (namespaceArray.isEmpty()) {
    //     qWarning() << "Failed to retrieve the namespaces array";
    //     return;
    // }

    // disconnect(opcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcDevice::namespacesArrayUpdated);

    // mGenericStructHandler.reset(new QOpcUaGenericStructHandler(mOpcUaClient));
    // connect(mGenericStructHandler.get(), &QOpcUaGenericStructHandler::initializeFinished, this, &OpcDevice::handleGenericStructHandlerInitFinished);
    // mGenericStructHandler->initialize();
}

void OpcDevice::getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    // int index = 0;
    // const std::array<const char *, 4> modes = {
    //     "Invalid",
    //     "None",
    //     "Sign",
    //     "SignAndEncrypt"
    // };

    // if (isSuccessStatus(statusCode)) {
    //     mEndpointList = endpoints;
    //     for (const auto &endpoint : endpoints) {
    //         if (endpoint.securityMode() >= modes.size()) {
    //             qWarning() << "Invalid security mode";
    //             continue;
    //         }

    //         const QString EndpointName = QStringLiteral("%1 (%2)")
    //                 .arg(endpoint.securityPolicy(), modes[endpoint.securityMode()]);
    //         index++;
    //     }
    // }

    // updateState();
}

void OpcDevice::findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode)
{
    // QOpcUaApplicationDescription server;

    // if (isSuccessStatus(statusCode)) {
    //     for (const auto &server : servers) {
    //         const auto urls = server.discoveryUrls();
    //         for (const auto &url : std::as_const(urls)) {
    //             qDebug() << url;
    //         }
    //     }
    // }

    // updateState();
}

void OpcDevice::onEventData(QJsonArray jsonArray)
{
    //setEventData(jsonArray);
}