#ifndef SINGLETONDATABASE_HPP
#define SINGLETONDATABASE_HPP

#include "Database.hpp"

class Singleton
{
public:
    Singleton(Singleton const&) = delete;
    Singleton& operator= (Singleton const&) = delete;

    static Database &database()
    {
        static Database database;
        return database;
    }
};

#endif // SINGLETONDATABASE_HPP
