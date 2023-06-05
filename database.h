#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5

//Перечисление полей данных
enum fieldsForConnect
{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

//Типы запросов
enum requestType
{
    requestAllFilms = 1,
    requestComedy   = 2,
    requestHorrors  = 3
};



class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void AddDataBase(QString driver, QString nameDB = "");
    void DisconnectFromDataBase(QString nameDb = "");
    void RequestToDB(QString request);
    QSqlError GetLastError(void);
    void ConnectToDataBase(QVector<QString> dataForConnect);
    QStringList getHeaders();

signals:
   void sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);
   void sig_SendStatusConnection(bool);

private:
   //QSqlQuery* query;
    QSqlDatabase* db;
    /*
        film_id - ИД записи;
        title - название фильма;
        description - описание фильма;
        release_year - год выпуска;
        language_id - язык фильма, внешний ключ(ВК)
        original_language_id - оригинальный язык фильма, ВК
        rental_duration - доступная продолжительность аренды фильма, дней;
        rental_rate - цена в $ за день;
        length - продолжительность фильма, мин;
        replacement_cost - цена замены фильма;
        rating - возрастной рейтинг фильма;
        last_update - служебная информация, последнее одновление записи;
        special_features - дополнительные особенности фильма;
        fulltext - ключевые слова для поиска фильма.
    */
    QStringList headers = {"ИД",
                           "Название фильма",
                           "Описание фильма",
                           "Год выпуска",
                           "Язык фильма",
                           "Оригинальный язык",
                           "Доступная аренда, дней",
                           "цена, $/день",
                           "Время фильма, мин",
                           "Цена замены фильма",
                           "Возрастной рейтинг",
                           "Служебная инфо",
                           "Дополнительно",
                           "Ключевые слова"};
};

#endif // DATABASE_H
