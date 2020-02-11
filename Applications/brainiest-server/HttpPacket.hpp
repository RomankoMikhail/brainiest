#ifndef HTTPPACKET_H
#define HTTPPACKET_H

#include "Cookie.hpp"
#include <QDateTime>
#include <QMap>
#include <QMimeType>
#include <QString>
#include <QStringList>
#include <QVector>

class HttpPacket
{
public:
    enum Code
    {
        CodeSwitchingProtocols  = 101, //!<
        CodeOk                  = 200, //!<
        CodeNotModified         = 304, //!<
        CodeBadRequest          = 400, //!<
        CodeForbidden           = 403, //!<
        CodeNotFound            = 404, //!<
        CodeRequestTimeout      = 408, //!<
        CodeLengthRequired      = 411, //!<
        CodePayloadTooLarge     = 413, //!<
        CodeUriTooLong          = 414, //!<
        CodeInternalServerError = 500, //!<
        CodeNotImplemented      = 501, //!<
        CodeServiceUnavailable  = 503, //!<
        CodeVersionNotSupported = 505  //!<
    };

    enum Method
    {
        MethodNotSupported, //!<
        MethodGet,          //!<
        MethodPost,         //!<
        MethodHead          //!<
    };

    HttpPacket() = default;

    /*!
     * \brief Формирует масив байт из HTTP пакета
     * \return Масив байт
     */
    QByteArray toByteArray();

    /*!
     * \brief Возвращает мажорную версию
     * \return
     */
    const int &major() const;
    void setMajor(int major);

    const int &minor() const;
    void setMinor(int minor);

    const Code &statusCode() const;
    void setStatusCode(const Code &statusCode);

    void setField(const QString &field, const QStringList &values);
    void setField(const QString &field, const QString &value);

    void addValue(const QString &field, const QString &value);
    void addValue(const QString &field, const QStringList &values);

    QStringList getValue(const QString &field) const;
    bool hasValue(const QString &field) const;

    const QMap<QString, QStringList> &fields() const;
    void setFields(const QMap<QString, QStringList> &fields);

    void clear();

    const QByteArray &data() const;
    void setData(const QByteArray &data);
    void setData(const QByteArray &data, const QMimeType &mimeType);

    const Method &method() const;
    void setMethod(const Method &method);

    const QString &url() const;
    void setUrl(const QString &url);

private:
    static QStringList parseField(const QString &fieldName, const QString &fieldValue);

    QString normilizeField(const QString &field) const;

    int mMajor       = 1;
    int mMinor       = 1;
    Code mStatusCode = CodeOk;
    Method mMethod   = MethodGet;

    QString mUrl = "/";

    QMap<QString, QStringList> mFields;
    QByteArray mData;
};

#endif // HTTPPACKET_H
