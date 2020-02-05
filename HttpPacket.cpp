#include "HttpPacket.h"
#include <QTextStream>

HttpPacket::HttpPacket()
{
}

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

    for (const auto &cookie : mServerCookies)
    {
        dataStream << cookie.toFieldValue() << "\r\n";
    }

    dataStream.flush();

    if (method() != MethodHead)
    {
        dataStream << "content-length: " << mData.size() << "\r\n";
        dataStream << "content-type: " << mMimeType.name() << "\r\n\r\n";
        dataStream.flush();
        dataStream.device()->write(mData);
    }

    return data;
}

int HttpPacket::major() const
{
    return mMajor;
}

void HttpPacket::setMajor(int httpMajor)
{
    mMajor = httpMajor;
}

int HttpPacket::minor() const
{
    return mMinor;
}

void HttpPacket::setMinor(int httpMinor)
{
    mMinor = httpMinor;
}

HttpPacket::Code HttpPacket::statusCode() const
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
    mFields.remove(field);
    addValue(field, value);
}

void HttpPacket::addValue(const QString &field, const QString &value)
{
    // Парсим специальные случаи
    QString normilizedField = normilizeField(field);

    if (normilizedField.contains("Cookie", Qt::CaseInsensitive))
    {
        mCookies.append(Cookie::fromFieldValue(value));
    }
    else if (normilizedField.contains("User-Agent", Qt::CaseInsensitive))
    {
        setUserAgent(value);
    }
    else
    {
        mFields[normilizedField].append(value);
    }
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

QMap<QString, QStringList> HttpPacket::fields() const
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
    mCookies.clear();
    mServerCookies.clear();
}

QByteArray HttpPacket::data() const
{
    return mData;
}

void HttpPacket::setData(const QByteArray &data)
{
    mData = data;
}

QMimeType HttpPacket::mimeType() const
{
    return mMimeType;
}

void HttpPacket::setMimeType(const QMimeType &mimeType)
{
    mMimeType = mimeType;
}

HttpPacket::Method HttpPacket::method() const
{
    return mMethod;
}

void HttpPacket::setMethod(const Method &method)
{
    mMethod = method;
}

QVector<Cookie> HttpPacket::cookies() const
{
    return mCookies;
}

void HttpPacket::setCookies(const QVector<Cookie> &cookies)
{
    mCookies = cookies;
}

QVector<Cookie> HttpPacket::serverCookies() const
{
    return mServerCookies;
}

void HttpPacket::setServerCookies(const QVector<Cookie> &serverCookies)
{
    mServerCookies = serverCookies;
}

void HttpPacket::addServerCookie(const Cookie &serverCookie)
{
    mServerCookies.append(serverCookie);
}

QString HttpPacket::normilizeField(const QString &field) const
{
    return field.trimmed().toLower();
}

QString HttpPacket::getUri() const
{
    return mUri;
}

void HttpPacket::setUri(const QString &uri)
{
    mUri = uri;
}

QString HttpPacket::userAgent() const
{
    return mUserAgent;
}

void HttpPacket::setUserAgent(const QString &userAgent)
{
    mUserAgent = userAgent;
}

QVector<Cookie> Cookie::fromFieldValue(const QString &value)
{
    QVector<Cookie> mCookies;

    QStringList listOfCookies = value.split("; ");

    for (const auto &string : listOfCookies)
    {
        QStringList components = string.split("=");

        Cookie cookie;

        if (components.size() > 1)
        {
            cookie.setName(components[0]);
            cookie.setValue(components[1]);

            mCookies.append(cookie);
        }
    }
    return mCookies;
}

QString Cookie::toFieldValue() const
{
    QString result = "Set-Cookie: ";

    result += mName + "=" + mValue;

    if (mMaxAge > 0)
        result += "; Max-Age=" + QString::number(mMaxAge);

    if (!mExpires.isNull())
        result += "; Expires=" + mExpires.toString("ddd, dd MMM yyyy hh:mm:ss t");

    if (!mDomain.isEmpty())
        result += "; Domain=" + mDomain;

    if (!mPath.isEmpty())
        result += "; Path=" + mPath;

    if (mHttpOnly)
        result += "; HttpOnly";

    if (mSameSite != PolicyDefault)
    {
        switch (mSameSite)
        {
        case PolicyLax:
            result += "; SameSite=Lax";
            break;

        case PolicyNone:
            result += "; SameSite=None";
            break;

        case PolicyStrict:
            result += "; SameSite=Strict";
            break;
        }
    }

    return result;
}

QString Cookie::name() const
{
    return mName;
}

void Cookie::setName(const QString &name)
{
    mName = name;
}

QString Cookie::value() const
{
    return mValue;
}

void Cookie::setValue(const QString &value)
{
    mValue = value;
}

QDateTime Cookie::expires() const
{
    return mExpires;
}

void Cookie::setExpires(const QDateTime &expires)
{
    mExpires = expires;
}

qint64 Cookie::maxAge() const
{
    return mMaxAge;
}

void Cookie::setMaxAge(const qint64 &maxAge)
{
    mMaxAge = maxAge;
}

QString Cookie::domain() const
{
    return mDomain;
}

void Cookie::setDomain(const QString &domain)
{
    mDomain = domain;
}

QString Cookie::path() const
{
    return mPath;
}

void Cookie::setPath(const QString &path)
{
    mPath = path;
}

bool Cookie::httpOnly() const
{
    return mHttpOnly;
}

void Cookie::setHttpOnly(bool httpOnly)
{
    mHttpOnly = httpOnly;
}

Cookie::SameSite Cookie::sameSite() const
{
    return mSameSite;
}

void Cookie::setSameSite(const SameSite &sameSite)
{
    mSameSite = sameSite;
}
