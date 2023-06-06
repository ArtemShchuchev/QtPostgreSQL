#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    /*
        Выделяем память под объекты классов.
        Объект QSqlDatabase является основным классом низкого уровня,
        в котором настраивается подключение к БД.
    */
    db = new QSqlDatabase();

    // Объект QSqlQuery отвечает за формирование запросов к БД
    query = new QSqlQuery();
    // QTableWidget необходим для формирования отображения ответа и передачи его в форму.
    //tableWidget = new QTableWidget();
    // содержит сообщения о ошибках
    err = new QSqlError();
}

DataBase::~DataBase()
{
    delete db;
    delete query;
    //delete tableWidget;
    delete err;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    /*
        Класс QSqlDatabase представляет из себя интерфейс между
        кодом и БД. Он организует соединение с БД. Для подключения
        в конструктор класса необходимо передать драйвер БД и имя.
        В объекте может храниться несколько подключений, они различаются именами.
    */
    *db = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    //Для подключения необходимо передать параметры БД в объект класса.
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

    //Для подключения к БД используется метод open(), который возвращает true в случае успешного подключения.
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
    //Сначала выбираем имя БД
    *db = QSqlDatabase::database(nameDb);
    //Метод close() закрывает соединение с БД
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

    /*
        Для фоомирования запроса будем использовать объект QSqlQuery.
        В конструктор передадим адрес объекта QSqlDatabase
    */
    *query = QSqlQuery(*db);
    /*
       Выполнение запроса выполняется при помощи
       метода exec. В случае успешного запроса он вернет true.
       Если возникает какая либо ошибка, ее можно посмотреть
       при помощи метода lastError. Этот метод возвращает
       экземпляр класса QSqlError.
    */
    QString request2 = "SELECT title, release_year FROM film";
    if (query->exec(request2) == false)
    {
        *err = query->lastError();
        qDebug() << *err;
    }
    else qDebug() << "Запрос выполнен!";

    //emit sig_SendStatusRequest(err);

    while(query->next())
    {
        qDebug() << query->value(0).toString() << " " << query->value(1).toString();
    }

    //emit sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);
}

/*!
 * \brief Метод читае ответ, полученный от БД при помощи QSqlQuery
 * \param answerType - тип ответа
 */
//void DataBase::ReadAnswerFromDB(int requestType)
//{
//    /*
//     * Используем оператор switch для разделения запросов
//    */
//    switch (requestType)
//    {
//    //Для наших запросов вид таблицы не поменяетя. Поэтому бужет единый обработчик.
//    case requestAllFilms:
//    case requestComedy:
//    case requestHorrors:
//    {
//        /*
//         * Объект tableWidget представляет из себя таблицу, которую можно отобразить
//         * в ПИ. Зfдадим ее параметры и заголовки. Мы получаем из БД название фильма, год выпуска и жанр
//        */

//        /* QSqlQuery
//         * После запроса к БД ответ помещается в объект QSqlQuery.
//         * Его необходимо считвать построчно. Для чтения ответа используется метод
//         * next, он работает следующим образом: при вызове метода указатель перемещается
//         * на следующую запись, в ответе. После получения ответа он указывает, если можно так
//         * выразится на -1 запись. Т.е. при первом вызове метода next указатель переместится
//         * на первую запись, а сам метод вернет true. В случае если указатель переместится
//         * за диапазон записей метод вернет false. Также он вернет false в случае отсутствия
//         * записей.
//         * Каждая запись является стройкой прочитанной из БД. Для перемещения по столбцам
//         * используется метод QSqlQuery value(индекс столбца).
//        */
//        /* QTableWidget
//         * Необходимо сформировать таблицу, которую потом передадим в MainWindow для отображения
//         * Для каждой записи прочитанной при помощи метода next будем добавлять строку в tableWidget
//         * и заполнять значениями из ответа. Каждая ячейка таблицы является объектом класса
//         * QTableWidgetItem. Т.е. для заполнения каждой ячеки мы должны создать экземпляр этого метода.
//         * Значение ячейки в таблицк устанавливается при помощи метода setItem.
//        */
//        /*
//         * После заполнения всей таблицы мы ее передаем в MainWindow при помощи сигнала.
//        */

//        tableWidget->setColumnCount(3);
//        tableWidget->setRowCount(0);
//        QStringList hdrs;
//        hdrs << "Название" << "Год выпуска" << "Жанр";
//        tableWidget->setHorizontalHeaderLabels(hdrs);

//        uint32_t conterRows = 0;

//        while(simpleQuery->next())
//        {
//            QString str;
//            tableWidget->insertRow(conterRows);

//            for(int i = 0; i<tableWidget->columnCount(); ++i)
//            {
//                str = simpleQuery->value(i).toString();
//                QTableWidgetItem *item = new QTableWidgetItem(str);
//                tableWidget->setItem(tableWidget->rowCount() - 1, i, item);
//            }
//            ++conterRows;
//        }
//        emit sig_SendDataFromDB(tableWidget, requestAllFilms);

//        break;
//    }

//    default:
//        break;
//    }
//}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return db->lastError();
}
