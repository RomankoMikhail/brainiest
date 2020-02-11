#include "WebServer.hpp"
#include "HttpParser.hpp"
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

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

    for (auto &context : mContexts)
    {
        context.socket->close();
    }

    mContexts.clear();
}

bool WebServer::listen(const QHostAddress &address, const quint16 &port)
{
    return mTcpServer.listen(address, port);
}

#include <QVariant>

void WebServer::onNewConnection()
{
    SocketContext context;

    context.socket = mTcpServer.nextPendingConnection();

    HttpParser *httpParser           = new HttpParser(context.socket);
    WebSocketParser *webSocketParser = new WebSocketParser(context.socket);

    context.socket->setProperty("httpParser", QVariant::fromValue(httpParser));
    context.socket->setProperty("webSocketParser", QVariant::fromValue(webSocketParser));
    context.socket->setProperty("context", QVariant::fromValue(context));

    connect(context.socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    connect(context.socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
    connect(httpParser, &HttpParser::httpParsed, this, &WebServer::onHttpPacketParsed);
    connect(webSocketParser, &WebSocketParser::frameReady, this,
            &WebServer::onWebSocketFrameParsed);

    context.socket->setProperty("protocol", ProtocolHttp);

    mContexts.push_back(context);
}

void WebServer::onDisconnect()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context;

    httpParser      = socket->property("httpParser").value<HttpParser *>();
    webSocketParser = socket->property("webSocketParser").value<WebSocketParser *>();
    context         = socket->property("context").value<SocketContext>();

    mContexts.removeOne(context);

    disconnect(httpParser, &HttpParser::httpParsed, this, &WebServer::onHttpPacketParsed);
    disconnect(webSocketParser, &WebSocketParser::frameReady, this,
               &WebServer::onWebSocketFrameParsed);
    disconnect(socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
}

void WebServer::onReadyRead()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context;

    httpParser      = socket->property("httpParser").value<HttpParser *>();
    webSocketParser = socket->property("webSocketParser").value<WebSocketParser *>();
    context         = socket->property("context").value<SocketContext>();

    Protocols socketProtocol = static_cast<Protocols>(socket->property("protocol").toInt());

    switch (socketProtocol)
    {
    case ProtocolHttp:
        httpParser->parse(socket, mMaxRequestSize);
        break;

    case ProtocolWebSocket:
        webSocketParser->parse(socket);
        break;
    }
}

void WebServer::onWebSocketFrameParsed(QTcpSocket *socket, const WebSocketFrame &frame)
{
    WebSocketFrame returnFrame;

    onWebsocketFrameFunction webSocketCallback;
    webSocketCallback = socket->property("webSocketCallback").value<onWebsocketFrameFunction>();

    SocketContext context;
    context = socket->property("context").value<SocketContext>();

    switch (frame.opcode())
    {
    case WebSocketFrame::OpcodePing:
        returnFrame.setOpcode(WebSocketFrame::OpcodePong);
        returnFrame.setIsFinalFrame(true);
        returnFrame.setData(frame.data());
        break;

    case WebSocketFrame::OpcodeClose:
        returnFrame.setOpcode(WebSocketFrame::OpcodeClose);
        returnFrame.setIsFinalFrame(true);
        returnFrame.setData(frame.data());

        socket->write(returnFrame.toByteArray());
        socket->close();
        return;

    default:
        returnFrame = webSocketCallback(context, frame);
        break;
    }

    socket->write(returnFrame.toByteArray());
    socket->flush();
}

void WebServer::onHttpPacketParsed(QTcpSocket *socket, const HttpPacket &packet)
{
    QString accessPath = packet.url();

    onHttpPacketFunction httpCallback = getHttpCallback(accessPath);

    SocketContext context;
    context = socket->property("context").value<SocketContext>();

    for (const auto &upgradeStrings : packet.getValue("upgrade"))
    {
        if (upgradeStrings.contains("websocket", Qt::CaseInsensitive))
        {
            onWebsocketFrameFunction webSocketCallback = getWebSocketCallback(accessPath);

            if (webSocketCallback == nullptr)
                break;

            socket->setProperty("webSocketCallback", QVariant::fromValue(webSocketCallback));
            upgradeToWebsocket(socket, packet.getValue("Sec-WebSocket-Key").first(), true);
            return;
        }
    }

    // Вызов Callback'a

    HttpPacket response;

    if (httpCallback != nullptr)
    {
        response = httpCallback(context, packet);
    }
    else
    {
        static QMimeDatabase mimeDatabase;

        response.setMethod(packet.method());
        response.setStatusCode(HttpPacket::CodeNotFound);
        response.setMajor(1);
        response.setMinor(1);

        response.setData("<h1>Not found</h1><p>The requested URL was not found</p>",
                         mimeDatabase.mimeTypeForName("text/html"));
    }

    bool connectionKeepAlive = false;

    for (const auto &connectionStrings : packet.getValue("connection"))
    {
        if (connectionStrings.contains("keep-alive", Qt::CaseInsensitive))
        {
            connectionKeepAlive = true;
            response.setField("connection", "keep-alive");
        }
    }

    socket->write(response.toByteArray());

    if (!connectionKeepAlive)
        socket->close();
}

void WebServer::registerHttpRoute(const QString &regularExpression, onHttpPacketFunction callback)
{
    mHttpRouter[regularExpression] = callback;
}

void WebServer::registerWebSocketRoute(const QString &regularExpression,
                                       onWebsocketFrameFunction callback)
{
    mWebsocketRouter[regularExpression] = callback;
}

onHttpPacketFunction WebServer::getHttpCallback(const QString &path)
{
    for (auto it = mHttpRouter.begin(); it != mHttpRouter.end(); ++it)
    {
        QRegExp regular(it.key());

        if (regular.exactMatch(path))
            return it.value();
    }

    return nullptr;
}

onWebsocketFrameFunction WebServer::getWebSocketCallback(const QString &path)
{
    for (auto it = mWebsocketRouter.begin(); it != mWebsocketRouter.end(); ++it)
    {
        QRegExp regular(it.key());

        if (regular.exactMatch(path))
            return it.value();
    }

    return nullptr;
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

    socket->setProperty("protocol", ProtocolWebSocket);

    socket->write(response.toByteArray());
}
