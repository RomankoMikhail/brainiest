#include "Cookie.hpp"

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

QString Cookie::toField() const
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
    default:
        break;
    }

    return result;
}

const QString &Cookie::name() const
{
    return mName;
}

void Cookie::setName(const QString &name)
{
    mName = name;
}

const QString &Cookie::value() const
{
    return mValue;
}

void Cookie::setValue(const QString &value)
{
    mValue = value;
}

const QDateTime &Cookie::expires() const
{
    return mExpires;
}

void Cookie::setExpires(const QDateTime &expires)
{
    mExpires = expires;
}

const qint64 &Cookie::maxAge() const
{
    return mMaxAge;
}

void Cookie::setMaxAge(const qint64 &maxAge)
{
    mMaxAge = maxAge;
}

const QString &Cookie::domain() const
{
    return mDomain;
}

void Cookie::setDomain(const QString &domain)
{
    mDomain = domain;
}

const QString &Cookie::path() const
{
    return mPath;
}

void Cookie::setPath(const QString &path)
{
    mPath = path;
}

const bool &Cookie::httpOnly() const
{
    return mHttpOnly;
}

void Cookie::setHttpOnly(bool httpOnly)
{
    mHttpOnly = httpOnly;
}

const Cookie::SameSite &Cookie::sameSite() const
{
    return mSameSite;
}

void Cookie::setSameSite(const SameSite &sameSite)
{
    mSameSite = sameSite;
}
