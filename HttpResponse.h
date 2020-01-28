#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QMap>
#include <QObject>

class HttpResponse : public QObject
{
    Q_OBJECT
public:
    explicit HttpResponse(QObject *parent = nullptr);

    QByteArray form();

    QList<QString> headers() const;
    QString getHeader(const QString &headerName) const;
    void setHeader(const QString &headerName, const QVariant &headerValue);
    void removeHeader(const QString &headerName);
    void clearHeaders();

    int getReturnCode() const;
    void setReturnCode(int returnCode);

    QByteArray getData() const;
    void setData(const QByteArray &data);

signals:

public slots:

private:
    QByteArray mData;
    int mReturnCode;
    QMap<QString, QString> mHeaders;
};

#endif // HTTPRESPONSE_H
