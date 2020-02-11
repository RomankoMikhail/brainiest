#include "WebServer.hpp"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>

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

QString calculateFileHash(const QString &pathToFile)
{
    QFile file(pathToFile);

    file.open(QFile::ReadOnly);

    if (file.isOpen())
    {
        QByteArray data = file.readAll();

        QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
        return hash.toBase64();
    }

    return "";
}

WebSocketFrame onEchoServer(SocketContext &context, const WebSocketFrame &frame)
{
    return frame;
}

static QString webrootDirectory = "webroot";

HttpPacket onFileSystemAccess(SocketContext &context, const HttpPacket &packet)
{
    QMimeDatabase mimeDatabase;
    HttpPacket response;

    QString accessUri = packet.url();
    if (accessUri.endsWith('/'))
        accessUri += "index.html";

    bool isNotRestrictedPath = isPathInDirectory(webrootDirectory + accessUri, webrootDirectory);

    if (!isNotRestrictedPath)
    {
        response.setStatusCode(HttpPacket::CodeForbidden);
    }
    else
    {
        QString path     = webrootDirectory + accessUri;
        QString fileHash = calculateFileHash(path);
        QFile file(path);

        file.open(QIODevice::ReadOnly);

        if (file.isOpen())
        {
            response.setStatusCode(HttpPacket::CodeOk);
            response.setData(file.readAll(), mimeDatabase.mimeTypeForFile(path));
        }
        else
            response.setStatusCode(HttpPacket::CodeNotFound);
    }

    return response;
}

int main(int argc, char *argv[])
{
    QMimeDatabase mimeDatabase;

    QString address = "127.0.0.1";
    int port        = 8080;

    QCoreApplication a(argc, argv);

    QFile configFile;
    configFile.setFileName("config.json");
    configFile.open(QIODevice::ReadOnly | QIODevice::Text);

    WebServer server;

    server.registerHttpRoute("^\\/((?!api)).*$", onFileSystemAccess);
    server.registerWebSocketRoute("/api/echo", onEchoServer);

    if (server.listen(QHostAddress(address), port))
    {
        qDebug() << "Server listening on" << address << ":" << QString::number(port);
    }
    else
    {
        return -1;
    }

    return a.exec();
}
