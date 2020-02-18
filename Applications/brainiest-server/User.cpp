#include "User.hpp"

#include "Singleton.hpp"
#include <QString>

const auto CREATE_USER = QString(R"(INSERT INTO `user`
                                 (`name`, `surname`, `patronymic`, `username`, `password`)
                                 VALUES(?, ?, ?, ?, ?))");

const auto UPDATE_NAME       = QString(R"(UPDATE `user` SET `name` = ? WHERE `id` = ?)");
const auto UPDATE_SURNAME    = QString(R"(UPDATE `user` SET `surname` = ? WHERE `id` = ?)");
const auto UPDATE_PATRONYMIC = QString(R"(UPDATE `user` SET `patronymic` = ? WHERE `id` = ?)");
const auto UPDATE_PASSWORD   = QString(R"(UPDATE `user` SET `password` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID = QString(R"(SELECT `name`, `surname`, `patronymic`, `username`, `password`
                                  FROM `user` WHERE `id` = ?)");

const auto SELECT_BY_USERNAME = QString(R"(SELECT `id` FROM `user` WHERE `username` = ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `user`)");

User User::create(const QString &name, const QString &surname, const QString &patronymic,
                  const QString &username, const QString &password)
{
    User newUser;

    newUser.mName       = name;
    newUser.mSurname    = surname;
    newUser.mPatronymic = patronymic;
    newUser.mUsername   = username;
    newUser.mPassword   = password;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE_USER);
    query.addBindValue(name);
    query.addBindValue(surname);
    query.addBindValue(patronymic);
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return User();
    }

    newUser.mId      = query.lastInsertId().toInt();
    newUser.mIsValid = true;

    return newUser;
}

User User::getById(const int &id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (query.next() == false)
        return User();

    User user;
    user.mId         = id;
    user.mName       = query.value(0).toString();
    user.mSurname    = query.value(1).toString();
    user.mPatronymic = query.value(2).toString();
    user.mUsername   = query.value(3).toString();
    user.mPassword   = query.value(4).toString();
    user.mIsValid    = true;

    return user;
}

User User::getByUsername(const QString &username)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_USERNAME);
    query.addBindValue(username);
    query.exec();

    if (query.next() == false)
        return User();

    return User::getById(query.value(0).toInt());
}

QList<int> User::getIds()
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_IDS);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

int User::id() const
{
    return mId;
}

QString User::name() const
{
    return mName;
}

void User::setName(const QString &name)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE_NAME);
    query.addBindValue(name);
    query.addBindValue(mId);
    if (query.exec())
        mName = name;
    else
        qDebug() << query.lastError();
}

QString User::surname() const
{
    return mSurname;
}

void User::setSurname(const QString &surname)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE_SURNAME);
    query.addBindValue(surname);
    query.addBindValue(mId);
    if (query.exec())
        mSurname = surname;
    else
        qDebug() << query.lastError();
}

QString User::patronymic() const
{
    return mPatronymic;
}

void User::setPatronymic(const QString &patronymic)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE_PATRONYMIC);
    query.addBindValue(patronymic);
    query.addBindValue(mId);
    if (query.exec())
        mPatronymic = patronymic;
    else
        qDebug() << query.lastError();
}

QString User::username() const
{
    return mUsername;
}

QString User::password() const
{
    return mPassword;
}

void User::setPassword(const QString &password)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE_PASSWORD);
    query.addBindValue(password);
    query.addBindValue(mId);

    if (query.exec())
        mPassword = password;
    else
        qDebug() << query.lastError();
}

bool User::isValid() const
{
    return mIsValid;
}
