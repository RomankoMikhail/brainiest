#ifndef HTTP_H
#define HTTP_H

#include <QMap>
#include <QObject>

class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QObject *parent = nullptr);
    explicit HttpRequest(const QByteArray &data, QObject *parent = nullptr);

    bool parse(const QByteArray &data);


    QList<QString> headers() const;
    QString getHeader(const QString &headerName) const;
    void setHeader(const QString &headerName, const QVariant &headerValue);

    QString getAccessPath() const;
    QString getAccessMethod() const;

signals:

public slots:

private:
    QString mAccessMethod;
    QString mAccessPath;
    QMap<QString, QString> mHeaders;
};

#endif // HTTP_H
