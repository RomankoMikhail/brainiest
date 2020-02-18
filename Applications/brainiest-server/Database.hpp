#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QString>
#include <QtSql>

class Database
{
public:
    Database();
    ~Database();

    QSqlError init(const QString &filename = "");

    QSqlDatabase database() const;

private:
    QSqlDatabase mDatabase;
};

#endif // DATABASE_HPP
