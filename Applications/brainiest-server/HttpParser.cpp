#include "HttpParser.hpp"

#include <QByteArray>
#include <QDebug>
#include <QString>

int messageCallback(http_parser *parser)
{
    HttpParser *caller = reinterpret_cast<HttpParser *>(parser->data);

    switch (parser->method)
    {
    case 1:
        caller->setMethod(HttpPacket::Method::MethodGet);
        break;

    case 2:
        caller->setMethod(HttpPacket::Method::MethodHead);
        break;

    case 3:
        caller->setMethod(HttpPacket::Method::MethodPost);
        break;

    default:
        caller->setMethod(HttpPacket::Method::MethodNotSupported);
        break;
    }

    caller->setHttpVersion(parser->http_major, parser->http_minor);

    caller->messageReady();
    return 0;
}

int urlCallback(http_parser *parser, const char *at, size_t length)
{
    HttpParser *caller = reinterpret_cast<HttpParser *>(parser->data);

    caller->setUrl(QString(QByteArray(at, length)));

    return 0;
}

int bodyCallback(http_parser *parser, const char *at, size_t length)
{
    HttpParser *caller = reinterpret_cast<HttpParser *>(parser->data);

    caller->setData(QByteArray(at, length));

    return 0;
}

int statusCallback(http_parser *parser, const char *at, size_t length)
{
    qDebug() << "STATUS:" << QByteArray(at, length);

    return 0;
}

int headerFieldCallback(http_parser *parser, const char *at, size_t length)
{
    HttpParser *caller = reinterpret_cast<HttpParser *>(parser->data);

    caller->setField(QString(QByteArray(at, length)));

    return 0;
}

int headerValueCallback(http_parser *parser, const char *at, size_t length)
{
    HttpParser *caller = reinterpret_cast<HttpParser *>(parser->data);

    caller->setValue(QString(QByteArray(at, length)));

    return 0;
}

HttpParser::HttpParser(QObject *parent) : QObject(parent)
{
    http_parser_settings_init(&mSettings);

    mSettings.on_url              = urlCallback;
    mSettings.on_body             = bodyCallback;
    mSettings.on_status           = statusCallback;
    mSettings.on_header_field     = headerFieldCallback;
    mSettings.on_header_value     = headerValueCallback;
    mSettings.on_message_complete = messageCallback;

    mParser       = new http_parser();
    mParser->data = this;

    http_parser_init(mParser, HTTP_REQUEST);
}

HttpParser::~HttpParser()
{
    delete mParser;
}

void HttpParser::parse(QIODevice *device, const qint64 &peekSize)
{
    QByteArray readed = device->peek(peekSize);

    qint64 skip = http_parser_execute(mParser, &mSettings, readed.data(), readed.size());

    if (device->isOpen())
        device->skip(skip);
}

void HttpParser::setUrl(const QString &url)
{
    mCurrentPacket.setUrl(url);
}

void HttpParser::setMethod(const HttpPacket::Method &method)
{
    mCurrentPacket.setMethod(method);
}

void HttpParser::setField(const QString &field)
{
    mCurrentField = field;
}

void HttpParser::setValue(const QString &value)
{
    mCurrentPacket.addValue(mCurrentField, value);
}

void HttpParser::setData(const QByteArray &data)
{
    mCurrentPacket.setData(data);
}

void HttpParser::setHttpVersion(const int &major, const int &minor)
{
    mCurrentPacket.setMajor(major);
    mCurrentPacket.setMinor(minor);
}

void HttpParser::messageReady()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(parent());

    if (socket != nullptr)
        emit httpParsed(socket, mCurrentPacket);

    mCurrentPacket = HttpPacket();
}

HttpPacket HttpParser::currentPacket() const
{
    return mCurrentPacket;
}

void HttpParser::setCurrentPacket(const HttpPacket &currentPacket)
{
    mCurrentPacket = currentPacket;
}
