#ifndef ACTION_HPP
#define ACTION_HPP

#include <QHash>
#include <QJsonObject>

class Action
{
public:
    enum ReturnCode
    {
        ReturnCodeOk                 = 0,
        ReturnCodeError              = -1,
        ReturnCodeNotFound           = -2,
        ReturnCodeNotEnoughArguments = -3,
    };

    using ActionCallback    = QJsonObject (*)(const QJsonObject &);
    using RequiredArguments = QList<QString>;

    void add(const QString &commandName, ActionCallback action, const RequiredArguments &args = {});
    QJsonObject exec(const QString &commandName, const QJsonObject &request);

    static QJsonObject formResponseFromCode(const ReturnCode &code);

private:
    QHash<QString, RequiredArguments> mRequiredArguments;
    QHash<QString, ActionCallback> mActions;
};

#endif // ACTION_HPP
