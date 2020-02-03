#include <QCoreApplication>

#include "WebServer.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WebServer server;

    if (server.listen(QHostAddress::Any))
    {
        qDebug() << "Server listening on default port";
    }
    else
    {
        a.exit(-1);
    }

    return a.exec();
}
