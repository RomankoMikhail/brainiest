#include "HttpPacket.hpp"
#include <QTextStream>

QByteArray HttpPacket::toByteArray()
{
    QByteArray data;
    QTextStream dataStream(&data);

    dataStream << "HTTP/" << major() << '.' << minor() << " " << statusCode() << "\r\n";

    for (auto iterator = mFields.begin(); iterator != mFields.end(); ++iterator)
    {
        for (const auto &value : iterator.value())
        {
            dataStream << iterator.key() << ": " << value << "\r\n";
        }
    }

    dataStream << "\r\n";
    dataStream.flush();

    if (method() != MethodHead)
    {
        dataStream.device()->write(mData);
    }

    return data;
}

const int &HttpPacket::major() const
{
    return mMajor;
}

void HttpPacket::setMajor(int httpMajor)
{
    mMajor = httpMajor;
}

const int &HttpPacket::minor() const
{
    return mMinor;
}

void HttpPacket::setMinor(int httpMinor)
{
    mMinor = httpMinor;
}

const HttpPacket::Code &HttpPacket::statusCode() const
{
    return mStatusCode;
}

void HttpPacket::setStatusCode(const Code &statusCode)
{
    mStatusCode = statusCode;
}

void HttpPacket::setField(const QString &field, const QStringList &values)
{
    mFields.remove(field);
    addValue(field, values);
}

void HttpPacket::setField(const QString &field, const QString &value)
{
    mFields.remove(normilizeField(field));
    addValue(field, value);
}

void HttpPacket::addValue(const QString &field, const QString &value)
{
    QString normilizedField = normilizeField(field);

    mFields[normilizedField].append(parseField(field, value));
}

void HttpPacket::addValue(const QString &field, const QStringList &values)
{
    for (const auto &value : values)
    {
        addValue(field, value);
    }
}

QStringList HttpPacket::getValue(const QString &field) const
{
    return mFields.value(normilizeField(field));
}

bool HttpPacket::hasValue(const QString &field) const
{
    return mFields.contains(normilizeField(field));
}

const QMap<QString, QStringList> &HttpPacket::fields() const
{
    return mFields;
}

void HttpPacket::setFields(const QMap<QString, QStringList> &fields)
{
    mFields = fields;
}

void HttpPacket::clear()
{
    mFields.clear();
    mData.clear();
}

const QByteArray &HttpPacket::data() const
{
    return mData;
}

void HttpPacket::setData(const QByteArray &data)
{
    setField("content-length", QString::number(data.size()));
    mData = data;
}

void HttpPacket::setData(const QByteArray &data, const QMimeType &mimeType)
{
    setField("content-type", mimeType.name());
    setData(data);
}

const HttpPacket::Method &HttpPacket::method() const
{
    return mMethod;
}

void HttpPacket::setMethod(const Method &method)
{
    mMethod = method;
}

QString HttpPacket::normilizeField(const QString &field) const
{
    return field.trimmed().toLower();
}

const QString &HttpPacket::url() const
{
    return mUrl;
}

void HttpPacket::setUrl(const QString &url)
{
    mUrl = url;
}

QStringList HttpPacket::parseField(const QString &fieldName, const QString &fieldValue)
{
    bool isSetCookie = fieldName.contains("set-cookie", Qt::CaseInsensitive);
    bool isUserAgent = fieldName.contains("user-agent", Qt::CaseInsensitive);
    bool isDate      = fieldName.contains("date", Qt::CaseInsensitive);
    bool isCookie    = fieldName.contains("cookie", Qt::CaseInsensitive);

    if (!isSetCookie && !isUserAgent && !isDate)
    {
        QStringList stringList;

        if (isCookie)
            stringList = fieldValue.split(';');
        else
            stringList = fieldValue.split(',');

        for (auto &string : stringList)
            string = string.trimmed();

        return stringList;
    }
    else
    {
        return QStringList(fieldValue);
    }
}
