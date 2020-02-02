#ifndef HTTP_H
#define HTTP_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>

class Http : public QObject
{
    Q_OBJECT
public:
    enum HttpCode
    {
        CodeSwitchingProtocols  = 101,
        CodeOk                  = 200,
        CodeForbidden           = 403,
        CodeNotFound            = 404,
        CodeLengthRequired      = 411,
        CodePayloadTooLarge     = 413,
        CodeInternalServerError = 500,
        CodeNotImplemented      = 501,
        CodeServiceUnavailable  = 503,
        CodeVersionNotSupported = 505
    };

    enum HttpMethod
    {
        MethodUnknown,
        MethodGet,
        MethodPost
    };

    explicit Http(QObject *parent = nullptr);

    QByteArray formResponse();
    qint64 parseRequest(const QByteArray &data);

    QList<QString> getFieldNames() const;

    QStringList getFieldValues(const QString &fieldName) const;
    QString getFieldValue(const QString &fieldName) const;

    void appendFieldValue(const QString &fieldName, const QString &fieldValue);

    void addFieldValue(const QString &fieldName, const QString &fieldValue);
    void addFieldValues(const QString &fieldName, const QStringList &fieldValues);

    void removeField(const QString &fieldName);
    void removeAllFields();

    bool hasField(const QString &fieldName) const;

    QByteArray getData() const;
    void setData(const QByteArray &data);
    void clearData();

    HttpMethod getAccessMethod() const;
    void setAccessMethod(const HttpMethod &accessMethod);

    QString getAccessPath() const;
    void setAccessPath(const QString &accessPath);

    HttpCode getHttpCode() const;
    void setHttpCode(const HttpCode &httpCode);

    void defaultCodeResponse(const Http::HttpCode &code);
signals:

public slots:

private:
    static QString transformFieldName(const QString &fieldName);

    QString mAccessPath;
    HttpMethod mAccessMethod;
    HttpCode mHttpCode;

    QByteArray mData;
    QMap<QString, QStringList> mFields;
};

#endif // HTTP_H
