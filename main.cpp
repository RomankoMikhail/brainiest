#include <QCoreApplication>

#include "WebServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WebServer server;

    if (server.listen(QHostAddress::LocalHost))
    {
        qDebug() << "Server listening on default port";
    }
    else
    {
        a.exit(-1);
    }

    return a.exec();
}
