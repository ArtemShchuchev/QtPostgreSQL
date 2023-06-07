#ifndef DATABASE_H
#define DATABASE_H

//#include <QTableWidget>
#include <QTableView>
#include <QObject>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QtConcurrent>
#include <QMessageBox>

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
// Я бы начал с 0, начинать с 1цы - не логично
// 0 - жанр не имеет значения
// цифры - конкретный жанр
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
    void RequestToDB(int requestIndex);
    void ReadAnswerFromDB( int requestIndex );
    QSqlError GetLastError(void);
    void ConnectToDataBase(QVector<QString> dataForConnect);
    QStringList getHeaders();
    QSqlDatabase& getMyDb() const;

signals:
   void sig_SendDataFromDB(const QTableView *tableView, int typeR);
   void sig_SendStatusConnection(bool);
   void sig_SendStatusRequest(QSqlError* err);

private:
    QSqlDatabase* db;
    QString tableName_str = "film";
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
    QSqlQuery* query;
    //QTableWidget* tableWidget;
    QTableView* tableView;
};

#endif // DATABASE_H
