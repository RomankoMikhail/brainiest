#ifndef APITOKEN_HPP
#define APITOKEN_HPP

#include <QString>
#include <QDateTime>

class ApiToken
{
public:
    static ApiToken create(const int &userId);
    static ApiToken create(const int &userId, const QDateTime &until);

    static ApiToken findByToken(const QString &token);

    ApiToken() = default;

    QString token() const;
    int userId() const;
    bool isValid() const;
    QDateTime validUntil() const;

    void invalidate();

private:
    static QString generate();

    bool mIsValid = false;
    int mUserId;
    QString mToken;
    QDateTime mValidUntil;
};

#endif // APITOKEN_HPP
