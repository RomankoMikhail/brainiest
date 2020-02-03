#ifndef HTTPPACKET_H
#define HTTPPACKET_H

#include <QDateTime>
#include <QMap>
#include <QMimeType>
#include <QString>
#include <QStringList>
#include <QVector>

class Cookie
{
public:
    enum SameSite
    {
        PolicyDefault,
        PolicyStrict,
        PolicyLax,
        PolicyNone
    };

    static QVector<Cookie> fromFieldValue(const QString &value);

    QString toFieldValue();

    QString name() const;
    void setName(const QString &name);

    QString value() const;
    void setValue(const QString &value);

    QDateTime expires() const;
    void setExpires(const QDateTime &expires);

    qint64 maxAge() const;
    void setMaxAge(const qint64 &maxAge);

    QString domain() const;
    void setDomain(const QString &domain);

    QString path() const;
    void setPath(const QString &path);

    bool httpOnly() const;
    void setHttpOnly(bool httpOnly);

    SameSite sameSite() const;
    void setSameSite(const SameSite &sameSite);

private:
    QString mName;
    QString mValue;

    QDateTime mExpires;
    qint64 mMaxAge = 0;
    QString mDomain;
    QString mPath;
    bool mHttpOnly = false;

    SameSite mSameSite;
};

class HttpPacket
{
public:
    enum Code
    {
        CodeContinue             = 100,
        CodeSwitchingProtocols   = 101,
        CodeProcessing           = 102,
        CodeOk                   = 200,
        CodeCreated              = 201,
        CodeAccepted             = 202,
        CodeNoContent            = 204,
        CodeResetContent         = 205,
        CodePartialContent       = 206,
        CodeMovedPermanently     = 301,
        CodeFound                = 302,
        CodeSeeOther             = 303,
        CodeNotModified          = 304,
        CodeUseProxy             = 305,
        CodeTemporaryRedirect    = 307,
        CodePermanentRedirect    = 308,
        CodeBadRequest           = 400,
        CodeUnauthorized         = 401,
        CodeForbidden            = 403,
        CodeNotFound             = 404,
        CodeMethodNotAllowed     = 405,
        CodeNotAcceptable        = 406,
        CodeRequestTimeout       = 408,
        CodeLengthRequired       = 411,
        CodePayloadTooLarge      = 413,
        CodeUriTooLong           = 414,
        CodeUnsupportedMediaType = 415,
        CodeUpgradeRequired      = 426,
        CodeTooManyRequests      = 431,
        CodeInternalServerError  = 500,
        CodeNotImplemented       = 501,
        CodeServiceUnavailable   = 503,
        CodeVersionNotSupported  = 505
    };

    enum Method
    {
        MethodNotSupported,
        MethodGet,
        MethodPost,
        MethodDelete,
        MethodPut,
        MethodHead
    };

    HttpPacket();

    QByteArray toByteArray();

    int major() const;
    void setMajor(int major);

    int minor() const;
    void setMinor(int minor);

    Code statusCode() const;
    void setStatusCode(const Code &statusCode);

    void setField(const QString &field, const QStringList &values);
    void setField(const QString &field, const QString &value);

    void addValue(const QString &field, const QString &value);
    void addValue(const QString &field, const QStringList &values);

    QStringList getValue(const QString &field);
    bool hasValue(const QString &field);


    QMap<QString, QStringList> fields() const;
    void setFields(const QMap<QString, QStringList> &fields);

    void clear();

    QByteArray data() const;
    void setData(const QByteArray &data);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);

    Method method() const;
    void setMethod(const Method &method);

    QVector<Cookie> cookies() const;
    void setCookies(const QVector<Cookie> &cookies);

    QVector<Cookie> serverCookies() const;
    void setServerCookies(const QVector<Cookie> &serverCookies);
    void addServerCookie(const Cookie &serverCookie);

    QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    QString getUri() const;
    void setUri(const QString &uri);

private:
    QString normilizeField(const QString &field);

    QString mUserAgent;

    int mMajor = 1;
    int mMinor = 1;
    Code mStatusCode = CodeOk;
    Method mMethod = MethodGet;

    QString mUri = "/";

    QVector<Cookie> mCookies;
    QVector<Cookie> mServerCookies;

    QMap<QString, QStringList> mFields;
    QByteArray mData;
    QMimeType mMimeType;
};

#endif // HTTPPACKET_H
