#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

HttpServer::HttpServer(QObject *parent) : QObject(parent)
{
    connect(&mTcpServer, &QTcpServer::newConnection, this, &HttpServer::onNewConnection);
}

HttpServer::~HttpServer()
{
    closeConnections();
}

void HttpServer::closeConnections()
{
    mTcpServer.pauseAccepting();

    for (auto socket : mClientSockets)
    {
        socket->close();
    }
}

bool HttpServer::listen(const QHostAddress &address, const quint16 &port)
{
    return mTcpServer.listen(address, port);
}

void HttpServer::onNewConnection()
{
    QTcpSocket *newSocket = mTcpServer.nextPendingConnection();

    connect(newSocket, &QTcpSocket::disconnected, this, &HttpServer::onDisconnect);
    connect(newSocket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);

    qDebug() << "Something connected";

    mClientSockets.push_back(newSocket);
}

void HttpServer::onDisconnect()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    mClientSockets.removeOne(socket);

    disconnect(socket, &QTcpSocket::disconnected, this, &HttpServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);

    qDebug() << "Something disconnected";
}

void HttpServer::onReadyRead()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    QByteArray array = socket->readAll();

    HttpRequest http;
    http.parse(array);

    qDebug() << "Access: " << http.getAccessPath();

    HttpResponse response;

    response.setReturnCode(200);
    response.setHeader("Content-Type", "text/html; charset=\"utf-8\"");
    response.setData("<h1>Hello world!</h1><h2>Are you impressed</h2>");

    socket->write(response.form());
    qDebug() << response.form();

    socket->close();
}
