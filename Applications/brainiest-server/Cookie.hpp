#ifndef COOKIE_HPP
#define COOKIE_HPP

#include <QDateTime>
#include <QVector>

/*!
 * \brief Класс предназначен для хранения куки-данных, которые могут приниматься от клинтов
 * или отправляться сервером
 */
class Cookie
{
public:
    /*!
     * \brief SameSite указывает, будут ли куки отправляться с запросами от различных источников или
     * нет. Установка этого параметра поможет обеспечить некоторую защиту от атак подделки запросов
     * от разных сайтов.
     */
    enum SameSite
    {
        PolicyDefault, //!< Отправка куки будет зависить от политики безопасности браузера
        PolicyStrict, //!< Отправка куки на другие сайты будет запрещена для всех запросов
        PolicyLax, //!< Отправка куки на другие сайты будет запрещена для небезопасных запросов
        PolicyNone //!< Отправка куки на другие сайты будет разрешена
    };

    /*!
     * \brief Формирует массив куки из значения поля заголовка HTTP запроса
     * \param value Значение поля заголовка
     * \return Массив куки
     *
     * \see toField
     */
    static QVector<Cookie> fromFieldValue(const QString &value);

    /*!
     * \brief Формирует строку заголовка HTTP ответа
     * \return Строка заголовка
     *
     * \see fromFieldValue
     */
    QString toField() const;

    /*!
     * \brief Возвращает название куки
     * \return Название куки
     *
     * \see setName
     */
    const QString &name() const;

    /*!
     * \brief Задает название куки
     * \param name Новое название куки
     *
     * \see name
     */
    void setName(const QString &name);

    /*!
     * \brief Возвращает значение куки
     * \return Значение куки
     *
     * \see setValue
     */
    const QString &value() const;

    /*!
     * \brief Задает значение куки
     * \param value Новое значение куки
     *
     * \see value
     */
    void setValue(const QString &value);

    /*!
     * \brief Возвращает дату, когда истекает время действия куки
     * \return Дата
     *
     * \see setExpires
     */
    const QDateTime &expires() const;

    /*!
     * \brief Задает новую дату, когда должно истечь время действия куки
     * \param expires Новая дата, которая определяет время действия куки
     *
     * \see expires
     */
    void setExpires(const QDateTime &expires);

    /*!
     * \brief Возвращает кол-во секунд времени действия куки
     * \return Кол-во секунд
     *
     * \see setMaxAge
     */
    const qint64 &maxAge() const;

    /*!
     * \brief Задает кол-во секунд времени действия куки
     * \param maxAge Новое кол-во секунд времени действия куки
     *
     * \see maxAge
     */
    void setMaxAge(const qint64 &maxAge);

    /*!
     * \brief Возвращает имя ресурса, которому будут отправляться куки
     * \return Имя ресурса
     *
     * \see setDomain
     */
    const QString &domain() const;

    /*!
     * \brief Задает имя ресурса, которому будут отправляться куки
     * \param domain Новое имя ресурса
     *
     * see domain
     */
    void setDomain(const QString &domain);

    /*!
     * \brief Возвращает путь, по которому будут отправляться куки
     * \return Путь
     *
     * \see setPath
     */
    const QString &path() const;

    /*!
     * \brief Задает путь, по которому будут отправляться куки
     * \param path Новый путь
     *
     * \see path
     */
    void setPath(const QString &path);

    /*!
     * \brief Возвращает значение политики безопасности, отвечающей за доступ к куки из
     * JavaScript
     * \return true разрешает доступ к куки из JavaScript и false запрещает
     * противном случае
     *
     * \see setHttpOnly
     */
    const bool &httpOnly() const;

    /*!
     * \brief Задает политику безопасности, отвечающего за доступ к куки из JavaScript
     * \param httpOnly true разрешает доступ к куки из JavaScript и false запрещает
     *
     * \see httpOnly
     */
    void setHttpOnly(bool httpOnly);

    /*!
     * \brief Возвращает политику безопасности отправки куки на стороние ресурсы
     * \return Значение политики безопастностт
     *
     * \see setSameSite, SameSite
     */
    const SameSite &sameSite() const;

    /*!
     * \brief Задает политику безопастности отправки куки на стороние ресурсы
     * \param sameSite Новое значение политики безопастности
     *
     * \see sameSite, SameSite
     */
    void setSameSite(const SameSite &sameSite);

private:
    QDateTime mExpires;
    SameSite mSameSite = PolicyStrict;

    QString mName;
    QString mValue;
    QString mDomain;
    QString mPath;

    qint64 mMaxAge = 0;
    bool mHttpOnly = true;
};
#endif // COOKIE_HPP
