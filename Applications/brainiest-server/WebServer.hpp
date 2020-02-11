#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "HttpPacket.hpp"
#include "HttpParser.hpp"
#include "SocketContext.hpp"
#include "WebSocketFrame.hpp"
#include "WebSocketParser.hpp"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = nullptr);
    ~WebServer();

    bool listen(const QHostAddress &address, const quint16 &port = 8080);
    void closeConnections();

    qint64 maxRequestSize() const;
    void setMaxRequestSize(const qint64 &maxRequestSize);

    void registerHttpRoute(const QString &pattern, onHttpPacketFunction callback);
    void registerWebSocketRoute(const QString &pattern, onWebsocketFrameFunction callback);

private slots:
    void onNewConnection();
    void onDisconnect();
    void onReadyRead();

    void onWebSocketFrameParsed(QTcpSocket *socket, const WebSocketFrame &frame);
    void onHttpPacketParsed(QTcpSocket *socket, const HttpPacket &packet);

    onHttpPacketFunction getHttpCallback(const QString &path);
    onWebsocketFrameFunction getWebSocketCallback(const QString &path);

private:
    enum Protocols
    {
        ProtocolHttp,
        ProtocolWebSocket
    };

    void upgradeToWebsocket(QTcpSocket *socket, const QString &webSocketKey, bool ieFix = false);

    QMap<QString, onHttpPacketFunction> mHttpRouter;
    QMap<QString, onWebsocketFrameFunction> mWebsocketRouter;

    QVector<SocketContext> mContexts;

    QTcpServer mTcpServer;

    qint64 mMaxRequestSize = 10485760; // 10 Mb
};

#endif // HTTPSERVER_H
