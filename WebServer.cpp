#include "WebServer.h"
#include "Http.h"

#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

bool isPathInDirectory(const QString &filePath, const QString &directoryPath)
{
    QString absouluteFilePath, absouluteDirectoryPath;

    absouluteFilePath      = QFileInfo(filePath).absolutePath();
    absouluteDirectoryPath = QDir(directoryPath).absolutePath();

    QString relativePath = QDir(absouluteDirectoryPath).relativeFilePath(absouluteFilePath);

    // Проверяем, есть ли выход за пределы католога
    if (relativePath.contains(".."))
        return false;

    // Проверяем, выходит ли католог на другой диск
    QFileInfoList rootDrivesPaths = QDir::drives();

    for (const auto &fileInfo : rootDrivesPaths)
    {
        if (relativePath.contains(fileInfo.path()))
            return false;
    }

    return true;
}

WebServer::WebServer(QObject *parent) : QObject(parent)
{
    connect(&mTcpServer, &QTcpServer::newConnection, this, &WebServer::onNewConnection);
}

WebServer::~WebServer()
{
    closeConnections();
}

void WebServer::closeConnections()
{
    mTcpServer.pauseAccepting();

    for (auto socket : mClientSockets)
    {
        socket->close();
    }
}

bool WebServer::listen(const QHostAddress &address, const quint16 &port)
{
    return mTcpServer.listen(address, port);
}

void WebServer::onNewConnection()
{
    QTcpSocket *newSocket = mTcpServer.nextPendingConnection();

    connect(newSocket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    connect(newSocket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);

    newSocket->setProperty("protocol", ProtocolHttp);
    mClientSockets.push_back(newSocket);
}

void WebServer::onDisconnect()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    mClientSockets.removeOne(socket);

    disconnect(socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
}

void WebServer::onReadyRead()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    Protocols socketProtocol = static_cast<Protocols>(socket->property("protocol").toInt());

    switch (socketProtocol)
    {
    case ProtocolHttp:
        httpRequest(socket);
        break;
    case ProtocolWebSocket:
        webSocketRequest(socket);
        break;
    }
}

void WebServer::httpRequest(QTcpSocket *socket)
{
    QByteArray array = socket->peek(maxRequestSize());

    Http request;
    qint64 requestRequestSize = request.parseRequest(array);

    if (requestRequestSize == 0)
        return;

    if (requestRequestSize > maxRequestSize())
    {
        Http response;
        response.setHttpCode(Http::CodePayloadTooLarge);
        socket->write(response.formResponse());
        socket->close();
        return;
    }

    socket->read(requestRequestSize);

    QStringList listOfCookies;
    QStringList listOfConnectionOptions;
    QStringList listOfUpgradeProtocols;
    QStringList listOfAcceptedEncoding;

    // Извлечь большую часть необходимой информации
    listOfCookies           = request.getFieldValues("Cookie");
    listOfUpgradeProtocols  = request.getFieldValue("Upgrade").split(",");
    listOfConnectionOptions = request.getFieldValue("Connection").split(",");
    listOfAcceptedEncoding  = request.getFieldValue("Accept-Encoding").split(",");

    for (QString &value : listOfCookies)
        value = value.trimmed();
    for (QString &value : listOfUpgradeProtocols)
        value = value.trimmed();
    for (QString &value : listOfConnectionOptions)
        value = value.trimmed();
    for (QString &value : listOfAcceptedEncoding)
        value = value.trimmed();

    Http response;
    response.defaultCodeResponse(Http::CodeNotFound);

    if (request.getAccessMethod() == Http::MethodUnknown)
        response.defaultCodeResponse(Http::CodeNotImplemented);

    bool shouldSocketBeClosed = true;

    if (listOfConnectionOptions.contains("Keep-Alive", Qt::CaseInsensitive))
    {
        response.appendFieldValue("Connection", "Keep-Alive");
        shouldSocketBeClosed = false;
    }

    if (response.getHttpCode() != Http::CodeNotImplemented)
    {
        bool isNotRestrictedPath =
            isPathInDirectory("webroot" + request.getAccessPath(), "webroot");

        if (!isNotRestrictedPath)
        {
            response.defaultCodeResponse(Http::CodeForbidden);
        }
        else
        {
            static QMimeDatabase db;

            QString path = "webroot" + request.getAccessPath();
            QFile file(path);

            file.open(QIODevice::ReadOnly);

            if(file.isOpen())
            {
                response.setHttpCode(Http::CodeOk);
                response.addFieldValue("content-type", db.mimeTypeForFile(path).name());
                response.setData(file.readAll());
            }
            else
                response.defaultCodeResponse(Http::CodeNotFound);
        }
    }

    socket->write(response.formResponse());

    if (shouldSocketBeClosed)
        socket->close();
}

void WebServer::webSocketRequest(QTcpSocket *socket)
{
}

qint64 WebServer::maxRequestSize() const
{
    return mMaxRequestSize;
}

void WebServer::setMaxRequestSize(const qint64 &maxRequestSize)
{
    mMaxRequestSize = maxRequestSize;
}
