#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

class HttpServer : public QObject
{
  Q_OBJECT
public:
  explicit HttpServer(QObject *parent = nullptr);
  ~HttpServer();

  bool listen(const QHostAddress &address, const quint16 &port = 8080);
  void closeConnections();


private slots:
  void onNewConnection();
  void onDisconnect();
  void onReadyRead();

private:

  QVector<QTcpSocket*> mClientSockets;
  QTcpServer mTcpServer;
};

#endif // HTTPSERVER_H
