#include "ApiToken.hpp"

#include <QDebug>
#include <QRandomGenerator>
#include "Singleton.hpp"

const auto CREATE_TOKEN = QString(R"(INSERT INTO `api_tokens`
                                 (`token`, `user_id`, `is_valid`, `valid_until`)
                                 VALUES(?, ?, 1, NULL))");

const auto CREATE_DATE_TOKEN = QString(R"(INSERT INTO `api_tokens`
                                 (`token`, `user_id`, `is_valid`, `valid_until`)
                                 VALUES(?, ?, 1, ?))");

const auto UPDATE_INVALID = QString(R"(UPDATE `api_tokens` SET `is_valid` = 0 WHERE `token` = ?)");

const auto SELECT_BY_TOKEN = QString(R"(SELECT `token`, `user_id`, `is_valid`, `valid_until`
                                     FROM `api_tokens` WHERE `token` = ?)");

ApiToken ApiToken::create(const int &userId)
{
    ApiToken newToken;

    newToken.mToken = generate();
    newToken.mValidUntil = QDateTime();
    newToken.mUserId = userId;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE_TOKEN);
    qDebug() << query.lastError();

    query.addBindValue(newToken.mToken);
    query.addBindValue(newToken.mUserId);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return ApiToken();
    }

    newToken.mIsValid = true;

    return newToken;
}

ApiToken ApiToken::create(const int &userId, const QDateTime &until)
{
    ApiToken newToken;

    newToken.mToken = generate();
    newToken.mValidUntil = until;
    newToken.mUserId = userId;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE_DATE_TOKEN);
    query.addBindValue(newToken.mToken);
    query.addBindValue(newToken.mUserId);
    query.addBindValue(newToken.mValidUntil);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return ApiToken();
    }

    newToken.mIsValid = true;

    return newToken;
}

ApiToken ApiToken::findByToken(const QString &token)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_TOKEN);
    query.addBindValue(token);
    query.exec();

    if (query.next() == false)
        return ApiToken();

    ApiToken newToken;

    newToken.mToken = token;
    newToken.mUserId = query.value(1).toInt();
    newToken.mIsValid = query.value(2).toBool();
    newToken.mValidUntil = query.value(3).toDateTime();

    return newToken;
}

QString ApiToken::token() const
{
    return mToken;
}

int ApiToken::userId() const
{
    return mUserId;
}

bool ApiToken::isValid() const
{
    if (mIsValid == false)
        return false;

    if (QDateTime::currentDateTime() > mValidUntil)
        return false;

    return true;
}

QDateTime ApiToken::validUntil() const
{
    return mValidUntil;
}

void ApiToken::invalidate()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE_INVALID);

    query.addBindValue(mToken);

    if (query.exec())
        mIsValid = false;
    else
        qDebug() << query.lastError();
}

QString ApiToken::generate()
{
    QString token = QString::number(QRandomGenerator::system()->generate64(), 16);
    token += QString::number(QRandomGenerator::system()->generate64(), 16);
    token += QString::number(QRandomGenerator::system()->generate64(), 16);
    token += QString::number(QRandomGenerator::system()->generate64(), 16);
    token = QString::number(QDateTime::currentSecsSinceEpoch(), 16) + token;

    return token;
}
