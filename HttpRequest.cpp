#include "HttpRequest.h"
#include <QDebug>
#include <QRegExp>
#include <QVariant>

HttpRequest::HttpRequest(QObject *parent) : QObject(parent)
{
}

HttpRequest::HttpRequest(const QByteArray &data, QObject *parent)
{
    parse(data);
}

bool HttpRequest::parse(const QByteArray &data)
{
    QString dataAsString(data);

    QStringList httpHeader = dataAsString.split("\r\n");

    int position = 0;
    for (QString str : httpHeader)
    {
        if (str.isEmpty())
            break;

        position++;
    }

    httpHeader.erase(httpHeader.begin() + position, httpHeader.end());

    QString request = httpHeader.first();
    httpHeader.removeFirst();

    QRegExp accessRegEx("([^\\s]+)\\s([^\\s]+)\\s([^\\s]+)");
    if (accessRegEx.exactMatch(request))
    {
        mAccessMethod = accessRegEx.cap(1);
        mAccessPath   = accessRegEx.cap(2);
    }
    else
    {
        return false;
    }

    for (const QString &str : httpHeader)
    {
        QRegExp headerRegEx("([^:]+):(.+)");

        if (headerRegEx.exactMatch(str))
            setHeader(headerRegEx.cap(1), headerRegEx.cap(2));
    }

    return true;
}

QList<QString> HttpRequest::headers() const
{
    return mHeaders.keys();
}

QString HttpRequest::getHeader(const QString &headerName) const
{
    if (mHeaders.contains(headerName))
        return mHeaders[headerName];

    return "";
}

void HttpRequest::setHeader(const QString &headerName, const QVariant &headerValue)
{
    mHeaders[headerName.trimmed()] = headerValue.toString().trimmed();
}

void HttpRequest::removeHeader(const QString &headerName)
{
    if (mHeaders.contains(headerName))
        mHeaders.remove(headerName);
}

void HttpRequest::clearHeaders()
{
    mHeaders.clear();
}

QString HttpRequest::getAccessPath() const
{
    return mAccessPath;
}

QString HttpRequest::getAccessMethod() const
{
    return mAccessMethod;
}
