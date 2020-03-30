#include "Actions.hpp"
#include "Singleton.hpp"
#include "User.hpp"
#include "ApiToken.hpp"

QJsonObject actionRegister(const QJsonObject &request)
{
    QJsonObject response;

    QString name       = request["args"]["name"].toString();
    QString surname    = request["args"]["surname"].toString();
    QString patronymic = request["args"]["patronymic"].toString();
    QString username   = request["args"]["username"].toString();
    QString password   = request["args"]["password"].toString();

    User newUser = User::create(name, surname, patronymic, username, password);

    if (!newUser.isValid())
        return Action::formResponseFromCode(Action::ReturnCodeError);

    return Action::formResponseFromCode(Action::ReturnCodeOk);
}

QJsonObject actionLogin(const QJsonObject &request)
{
    QJsonObject response;

    QString username = request["args"]["username"].toString();
    QString password = request["args"]["password"].toString();

    User user = User::getByUsername(username);

    if (user.isValid() && user.password() == password)
    {
        response = Action::formResponseFromCode(Action::ReturnCodeOk);

        QDateTime until = QDateTime::currentDateTime().addSecs(24 * 60 * 60);

        response["token"] = ApiToken::create(user.id(), until).token();
        response["until"] = until.toSecsSinceEpoch();

        return response;
    }

    return Action::formResponseFromCode(Action::ReturnCodeError);
}

void registerActions()
{
    Singleton::action().add("register", actionRegister,
                            {"name", "surname", "patronymic", "username", "password"});
    Singleton::action().add("login", actionLogin, {"username", "password"});
}
