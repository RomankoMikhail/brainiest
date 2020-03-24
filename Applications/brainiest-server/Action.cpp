#include "Action.hpp"

void Action::add(const QString &commandName, Action::ActionCallback action, const RequiredArguments &args)
{
    mActions.insert(commandName, action);
    mRequiredArguments.insert(commandName, args);
}

QJsonObject Action::exec(const QString &commandName, const QJsonObject &request)
{
    auto action = mActions.find(commandName);
    auto requiredArguments = mRequiredArguments.find(commandName);

    if (action == mActions.end())
        return formResponseFromCode(ReturnCodeNotFound);

    if(requiredArguments != mRequiredArguments.end())
    {
        if(!request.contains("args") || !request["args"].isObject())
            return formResponseFromCode(ReturnCodeNotEnoughArguments);

        for(auto argumentName : *requiredArguments)
        {
            if(!request["args"].toObject().contains(argumentName))
                return formResponseFromCode(ReturnCodeNotEnoughArguments);
        }
    }

    return (*action)(request);
}

QJsonObject Action::formResponseFromCode(const Action::ReturnCode &code)
{
    QJsonObject response;

    response.insert("code", code);

    return response;
}
