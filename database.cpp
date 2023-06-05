#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    db = new QSqlDatabase();
}

DataBase::~DataBase()
{
    delete db;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *db = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    db->setHostName(data[hostName]);
    db->setDatabaseName(data[dbName]);
    db->setUserName(data[login]);
    db->setPassword(data[pass]);
    db->setPort(data[port].toInt());


    ///Тут должен быть код ДЗ

    qDebug() << "host name: " << db->hostName()
             << "\ndbName: " << db->databaseName()
             << "\nlogin: " << db->userName()
             << "\npassword: " << db->password()
             << "\nport: " << db->port();

    bool err = db->open();
    if (err) qDebug() << "База подключена!";
    emit sig_SendStatusConnection(err);
}

QStringList DataBase::getHeaders()
{
    return headers;
}

/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    *db = QSqlDatabase::database(nameDb);
    db->close();
}

/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QString request)
{
    ///Тут должен быть код ДЗ
    QSqlQuery query;
    //*query = QSqlQuery(*dataBase);
    if (query.exec(request) == false) qDebug() << query.lastError();
    else qDebug() << "Запрос выполнен!";
    while(query.next())
    {
        qDebug() << query.value(0).toString() << " " << query.value(1).toString();
    }

    //emit sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return db->lastError();
}
