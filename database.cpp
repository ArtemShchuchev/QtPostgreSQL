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
    var = new QVariant();

    // Добавим БД используя стандартный драйвер PSQL и зададим имя.
    AddDataBase(POSTGRE_DRIVER, DB_NAME);
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
void DataBase::AddDataBase(const QString &driver, const QString &nameDB)
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
void DataBase::ConnectToDataBase(const QVector<QString> &data)
{
    //Для подключения необходимо передать параметры БД в объект класса.
    db->setHostName(data[hostName]);
    db->setDatabaseName(data[dbName]);
    db->setUserName(data[login]);
    db->setPassword(data[pass]);
    db->setPort(data[port].toInt());

    ///Тут должен быть код ДЗ
    //Для подключения к БД используется метод open(), который возвращает true в случае успешного подключения.
    auto connection = [this]{ emit sig_SendStatusConnection(db->open()); };
    auto future = QtConcurrent::run(connection);
}

/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(const QString& nameDb)
{
    //Сначала выбираем имя БД
    *db = QSqlDatabase::database(nameDb);
    //Метод close() закрывает соединение с БД
    db->close();
}

/*!
 * \brief Метод формирует запрос к БД.
 * \param requestIndex - индекс SQL запроса
 * \return
 */
void DataBase::RequestToDB(int requestIndex)
{
    ///Тут должен быть код ДЗ
    QString request =
            "SELECT title, description FROM " + tableName_str + " f\
            JOIN film_category fc on f.film_id = fc.film_id\
            JOIN category c on c.category_id = fc.category_id WHERE c.name = ";

    // Используем оператор switch для разделения запросов
    switch (requestIndex + 1)
    {
        case requestAllFilms:
        {
            //qDebug() << "Получаю все фильмы";
            /* Производим инициализацию модели представления данных
             * с установкой имени таблицы в базе данных, по которому
             * будет производится обращение в таблице
             * */
            model = new QSqlTableModel(this, *db);
            model->setTable(tableName_str);
            model->setEditStrategy(QSqlTableModel::OnManualSubmit);

            // Удаляю лишние колонки
            const int startDelColumn(3);
            int column(model->columnCount()-startDelColumn);
            QModelIndex lastColIdx = model->index(0, model->columnCount(), QModelIndex());
            model->removeColumns(startDelColumn, column, lastColIdx);

            // Устанавливаем названия колонок в таблице с сортировкой данных
            for (int i(0); i < startDelColumn; ++i)
            {
                model->setHeaderData(i, Qt::Horizontal, headers[i]);
            }

            // Устанавливаем сортировку по возрастанию данных по 1ой колонке
            model->setSort(1, Qt::AscendingOrder);

            // Если возникает какая либо ошибка, ее можно посмотреть при помощи метода lastError.
            emit sig_SendStatusRequest(model->lastError());
            break;
        }

        case requestComedy:
        {
            //qDebug() << "Получаю комедии";
            request += "'Comedy'";
            qModel = new QSqlQueryModel(this);
            qModel->setQuery(request, *db);
            qModel->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
            qModel->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
            qModel->sort(1, Qt::AscendingOrder);

            emit sig_SendStatusRequest(qModel->lastError());
            break;
        }

        case requestHorrors:
        {
            //qDebug() << "Получаю ужасы";
            request += "'Horror'";
            qModel = new QSqlQueryModel(this);
            qModel->setQuery(request, *db);
            qModel->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
            qModel->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
            qModel->sort(1, Qt::AscendingOrder);

            emit sig_SendStatusRequest(qModel->lastError());
            break;
        }
        default:
        break;
    }
}

/*!
 * \brief Метод возвращает модель таблицы БД
 * \param requestIndex - индекс SQL запроса
 */
void DataBase::ReadAnswerFromDB(int requestIndex)
{
    switch (requestIndex + 1)
    {
    case requestAllFilms:
        var->setValue(model);
        emit sig_SendDataFromDB(var);
        break;

    case requestComedy:
    case requestHorrors:
        var->setValue(qModel);
        emit sig_SendDataFromDB(var);
        break;
    default:
        break;
    }
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return db->lastError();
}
