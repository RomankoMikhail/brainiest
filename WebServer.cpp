#include "WebServer.hpp"

#include "HttpParser.hpp"
#include <QCryptographicHash>
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

    for (auto iterator = mClientSockets.begin(); iterator != mClientSockets.end(); ++iterator)
    {
        iterator.key()->close();
    }

    mClientSockets.clear();
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

    SocketContext context;
    context.httpParser = new HttpParser(newSocket);
    connect(context.httpParser, &HttpParser::httpParsed, this, &WebServer::onHttpPacketParsed);

    mClientSockets[newSocket] = context;
}

void WebServer::onDisconnect()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    if (mClientSockets[socket].httpParser != nullptr)
    {
        disconnect(mClientSockets[socket].httpParser, &HttpParser::httpParsed, this,
                   &WebServer::onHttpPacketParsed);
    }

    if (mClientSockets[socket].webSocketParser != nullptr)
    {
        disconnect(mClientSockets[socket].webSocketParser, &WebSocketParser::frameReady, this,
                   &WebServer::onWebSocketFrameParsed);
    }

    mClientSockets.remove(socket);

    disconnect(socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
}

void WebServer::onReadyRead()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    SocketContext context = mClientSockets[socket];

    Protocols socketProtocol = static_cast<Protocols>(socket->property("protocol").toInt());

    switch (socketProtocol)
    {
    case ProtocolHttp:
        context.httpParser->parse(socket, mMaxRequestSize);
        break;

    case ProtocolWebSocket:
        context.webSocketParser->parse(socket);
        break;
    }
}

void WebServer::onWebSocketFrameParsed(QTcpSocket *socket, WebSocketFrame frame)
{
    WebSocketFrame returnFrame;
    returnFrame.setOpcode(WebSocketFrame::OpcodeText);
    returnFrame.setData(frame.data());
    returnFrame.setIsFinalFrame(true);

    socket->write(returnFrame.toByteArray());

    socket->flush();
}

void WebServer::onHttpPacketParsed(QTcpSocket *socket, HttpPacket packet)
{
    static QMimeDatabase mimeDatabase;

    HttpPacket response;
    response.setMethod(packet.method());
    response.setStatusCode(HttpPacket::CodeServiceUnavailable);
    response.setMajor(1);
    response.setMinor(1);
    response.setMimeType(mimeDatabase.mimeTypeForName("text/html"));

    bool connectionKeepAlive = false;

    for (auto upgradeStrings : packet.getValue("upgrade"))
    {
        if (upgradeStrings.contains("websocket", Qt::CaseInsensitive))
        {
            qDebug() << "Upgrading!";
            upgradeToWebsocket(socket, packet.getValue("Sec-WebSocket-Key").first(), true);
            return;
        }
    }

    for (auto connectionStrings : packet.getValue("connection"))
    {
        if (connectionStrings.contains("keep-alive", Qt::CaseInsensitive))
        {
            connectionKeepAlive = true;
            response.setField("connection", "keep-alive");
        }
    }

    bool isNotRestrictedPath = isPathInDirectory("webroot" + packet.getUri(), "webroot");

    if (!isNotRestrictedPath)
    {
        response.setStatusCode(HttpPacket::CodeForbidden);
    }
    else
    {
        QString path = "webroot" + packet.getUri();
        QFile file(path);

        file.open(QIODevice::ReadOnly);

        if (file.isOpen())
        {
            response.setStatusCode(HttpPacket::CodeOk);
            response.setMimeType(mimeDatabase.mimeTypeForFile(path));
            response.setData(file.readAll());
        }
        else
            response.setStatusCode(HttpPacket::CodeNotFound);
    }

    socket->write(response.toByteArray());

    if (!connectionKeepAlive)
        socket->close();
}

void WebServer::webSocketRequest(QTcpSocket *socket)
{
    for (auto child : socket->children())
    {
        WebSocketParser *parser = dynamic_cast<WebSocketParser *>(child);

        if (parser == nullptr)
            continue;

        qDebug() << "Should parse?";

        parser->parse(socket);
    }
}

qint64 WebServer::maxRequestSize() const
{
    return mMaxRequestSize;
}

void WebServer::setMaxRequestSize(const qint64 &maxRequestSize)
{
    mMaxRequestSize = maxRequestSize;
}

void WebServer::upgradeToWebsocket(QTcpSocket *socket, const QString &webSocketKey, bool ieFix)
{
    QString magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    QString stringKey   = webSocketKey + magicString;

    QByteArray resultOfHash =
        QCryptographicHash::hash(stringKey.toUtf8(), QCryptographicHash::Sha1);
    QString acceptString = resultOfHash.toBase64();

    HttpPacket response;

    response.addValue("Sec-WebSocket-Accept", acceptString);
    response.addValue("Connection", "Upgrade");

    if (ieFix)
        response.addValue("Upgrade", "Websocket");
    else
        response.addValue("Upgrade", "websocket");

    response.setStatusCode(HttpPacket::CodeSwitchingProtocols);

    mClientSockets[socket].webSocketParser = new WebSocketParser(socket);

    connect(mClientSockets[socket].webSocketParser, &WebSocketParser::frameReady, this,
            &WebServer::onWebSocketFrameParsed);

    socket->setProperty("protocol", ProtocolWebSocket);

    socket->write(response.toByteArray());
}
