#ifndef QUESTION_HPP
#define QUESTION_HPP

#include <QList>
#include <QString>

class Question
{
public:
    static QList<QString> getListOfThemes();
    static Question create(QString theme, QString question, QString correctAnswer,
                           QList<QString> wrongAnswer);

    static Question findById(const int &id);
    static QList<int> findIdsWithTheme(const QString &theme);

    Question();

    int id() const;

    QString theme() const;
    void setTheme(const QString &theme);

    QString question();
    void setQuestion(const QString &question);

    QString correctAnswer() const;
    void setCorrectAnswer(const QString &answers);

    QList<QString> wrongAnswer() const;
    void setWrongAnswer(const QList<QString> &answers);
};

#endif // QUESTION_HPP
