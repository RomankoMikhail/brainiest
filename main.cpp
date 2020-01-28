#include <QCoreApplication>

#include "HttpServer.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  HttpServer server;

  if (server.listen(QHostAddress::LocalHost)) {
    qDebug() << "Server listening on default port";
  } else {
    a.exit(-1);
  }

  return a.exec();
}
