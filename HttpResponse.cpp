#include "HttpResponse.h"
#include <QDebug>
#include <QRegExp>
#include <QVariant>

HttpResponse::HttpResponse(QObject *parent) : QObject(parent)
{
}

QByteArray HttpResponse::form()
{
    QByteArray returnArray;
    returnArray.append("HTTP/1.0 ");
    returnArray.append(QString::number(mReturnCode) + "\r\n");

    for (const QString &key : mHeaders.keys())
    {
        returnArray.append(key + ":");
        returnArray.append(getHeader(key) + "\r\n");
    }
    returnArray.append("\r\n");
    returnArray.append(mData);

    return returnArray;
}

QList<QString> HttpResponse::headers() const
{
    return mHeaders.keys();
}

QString HttpResponse::getHeader(const QString &headerName) const
{
    if (mHeaders.contains(headerName))
        return mHeaders[headerName];

    return "";
}

void HttpResponse::setHeader(const QString &headerName, const QVariant &headerValue)
{
    mHeaders[headerName.trimmed()] = headerValue.toString().trimmed();
}

void HttpResponse::removeHeader(const QString &headerName)
{
    if (mHeaders.contains(headerName))
        mHeaders.remove(headerName);
}

void HttpResponse::clearHeaders()
{
    mHeaders.clear();
}

int HttpResponse::getReturnCode() const
{
    return mReturnCode;
}

void HttpResponse::setReturnCode(int returnCode)
{
    mReturnCode = returnCode;
}

QByteArray HttpResponse::getData() const
{
    return mData;
}

void HttpResponse::setData(const QByteArray &data)
{
    mData = data;
}
