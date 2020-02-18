#ifndef USER_HPP
#define USER_HPP

#include <QString>

class User
{
public:
    static User create(const QString &name, const QString &surname, const QString &patronymic,
                       const QString &username, const QString &password);

    static User getById(const int &id);
    static User getByUsername(const QString &username);

    static QList<int> getIds();

    User() = default;
    int id() const;

    QString name() const;
    void setName(const QString &name);

    QString surname() const;
    void setSurname(const QString &surname);

    QString patronymic() const;
    void setPatronymic(const QString &patronymic);

    QString username() const;

    QString password() const;
    void setPassword(const QString &password);

    bool isValid() const;

private:
    bool mIsValid = false;
    int mId       = -1;
    QString mName;
    QString mSurname;
    QString mPatronymic;
    QString mUsername;
    QString mPassword;
};

#endif // USER_HPP
