#include "Http.h"
#include <QVariant>

#include <QDebug>

int calculateHeadersSize(const QByteArray &array)
{
    enum States
    {
        StateNormal,
        StateFoundCR,
        StateFoundLF,
        StateFoundSecondCR
    } state = StateNormal;

    for (auto it = array.begin(); it != array.end(); ++it)
    {
        switch (state)
        {
        case StateNormal:
            if (*it == '\r')
                state = StateFoundCR;
            break;

        case StateFoundCR:
            if (*it == '\n')
                state = StateFoundLF;
            else
                state = StateNormal;
            break;

        case StateFoundLF:
            if (*it == '\r')
                state = StateFoundSecondCR;
            else
                state = StateNormal;
            break;

        case StateFoundSecondCR:
            if (*it == '\n')
                return std::distance(array.begin(), it) + 1;
            else
                state = StateNormal;
            break;
        }
    }
    return 0;
}

Http::Http(QObject *parent) : QObject(parent)
{
    setAccessPath("/");
    setAccessMethod(MethodUnknown);
}

QByteArray Http::formResponse()
{
    QByteArray returnArray;

    removeField("Content-Length");
    removeField("Content-Encoding");

    addFieldValue("Content-Length", QString::number(mData.size()));

    returnArray.append("HTTP/1.1 ");
    returnArray.append(QString::number(getHttpCode()) + "\r\n");

    for (const QString &key : getFieldNames())
    {
        for (const QString &value : getFieldValues(key))
        {
            returnArray.append(key + ":" + value + "\r\n");
        }
    }
    returnArray.append("\r\n");
    returnArray.append(mData);

    return returnArray;
}

qint64 Http::parseRequest(const QByteArray &data)
{
    qDebug() << data;
    int httpHeaderSize = calculateHeadersSize(data);

    if (httpHeaderSize == 0)
        return 0;

    QString httpHeaderString = data.left(httpHeaderSize);
    QStringList httpHeader   = httpHeaderString.split("\r\n");

    QString request = httpHeader.first();
    httpHeader.removeFirst();

    QRegExp accessRegEx("([^\\s]+)\\s([^\\s]+)\\s([^\\s]+)");
    if (accessRegEx.exactMatch(request))
    {
        QString method = accessRegEx.cap(1).toUpper();

        setAccessMethod(MethodUnknown);

        if (method == "GET")
            setAccessMethod(MethodGet);

        else if (method == "POST")
            setAccessMethod(MethodPost);

        setAccessPath(accessRegEx.cap(2));
    }
    else
        return 0;

    for (const QString &str : httpHeader)
    {
        QRegExp headerRegEx("([^:]+):(.+)");

        if (headerRegEx.exactMatch(str))
            addFieldValue(headerRegEx.cap(1), headerRegEx.cap(2));
    }

    if (!hasField("content-length"))
        addFieldValue("content-length", "0");

    int contentLength = getFieldValue("content-length").toInt();

    setData(data.mid(httpHeaderSize, contentLength));

    return httpHeaderSize + contentLength;
}

QList<QString> Http::getFieldNames() const
{
    return mFields.keys();
}

QStringList Http::getFieldValues(const QString &fieldName) const
{
    return mFields.value(transformFieldName(fieldName));
}

QString Http::getFieldValue(const QString &fieldName) const
{
    QStringList list = getFieldValues(fieldName);
    return list.value(0, "");
}

void Http::appendFieldValue(const QString &fieldName, const QString &fieldValue)
{
    if (!hasField(fieldName))
        addFieldValue(fieldName, fieldValue);
    else
    {
        mFields[transformFieldName(fieldName)][0] += "," + fieldValue;
    }
}

void Http::addFieldValue(const QString &fieldName, const QString &fieldValue)
{
    mFields[transformFieldName(fieldName)].append(fieldValue.trimmed());
}

void Http::addFieldValues(const QString &fieldName, const QStringList &fieldValues)
{
    mFields[transformFieldName(fieldName)].append(fieldValues);
}

void Http::removeField(const QString &fieldName)
{
    mFields.remove(transformFieldName(fieldName));
}

void Http::removeAllFields()
{
    mFields.clear();
}

bool Http::hasField(const QString &fieldName) const
{
    return mFields.contains(transformFieldName(fieldName));
}

QByteArray Http::getData() const
{
    return mData;
}

void Http::setData(const QByteArray &data)
{
    mData = data;
}

void Http::clearData()
{
    mData.clear();
}

Http::HttpMethod Http::getAccessMethod() const
{
    return mAccessMethod;
}

void Http::setAccessMethod(const HttpMethod &accessMethod)
{
    mAccessMethod = accessMethod;
}

QString Http::getAccessPath() const
{
    return mAccessPath;
}

void Http::setAccessPath(const QString &accessPath)
{
    mAccessPath = accessPath.trimmed();
}

Http::HttpCode Http::getHttpCode() const
{
    return mHttpCode;
}

void Http::setHttpCode(const HttpCode &httpCode)
{
    mHttpCode = httpCode;
}

void Http::defaultCodeResponse(const Http::HttpCode &code)
{
    setHttpCode(code);
    setData(("<h1>" + QString::number(code) + "</h1>").toUtf8());
}

QString Http::transformFieldName(const QString &fieldName)
{
    return fieldName.trimmed().toLower();
}
