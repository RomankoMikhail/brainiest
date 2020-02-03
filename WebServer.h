#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Http.h"
#include "WebSocketParser.h"
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

private slots:
    void onNewConnection();
    void onDisconnect();
    void onReadyRead();

    void onWebSocketFrameParsed(QTcpSocket *socket, WebSocketFrame frame);

private:
    enum Protocols
    {
        ProtocolHttp,
        ProtocolWebSocket
    };

    void httpRequest(QTcpSocket *socket);
    void webSocketRequest(QTcpSocket *socket);

    QMap<QString, QFunctionPointer> mPaths;
    QVector<QTcpSocket *> mClientSockets;
    QTcpServer mTcpServer;
    qint64 mMaxRequestSize = 10485760; // 10 Mb
};

#endif // HTTPSERVER_H
