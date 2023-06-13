#include "database.h"

DataBase::DataBase(QObject *parent,
                   const QString& driver, const QString& nameDB)
    : QObject{parent}, model(nullptr), qModel(nullptr)
{
    /*
        Выделяем память под объекты классов.
        Объект QSqlDatabase является основным классом низкого уровня,
        в котором настраивается подключение к БД.
    */
    db = new QSqlDatabase();
    modelVariant = new QVariant();

    // Добавим БД используя стандартный драйвер PSQL и зададим имя.
    AddDataBase(driver, nameDB);
}

DataBase::~DataBase()
{
    delete db;
    delete modelVariant;
    deleteModels();
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

void DataBase::deleteModels()
{
    if (model){
        delete model;
        model = nullptr;
    }
    if (qModel){
        delete qModel;
        qModel = nullptr;
    }
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
    emit sig_SendStatusConnection(db->open());
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
    /*
     * Получается, самый правильный способ!
     *
     * Можно так же динамически выделять память под "Модели" в методе запроса, но
     * в качесве родителя должен быть указан (0), не (this), а также перед каждым
     * вызовом метода (в начале) проверять наличие уже созданных моделей и освобождать
     * память (if (model) delete model; model = nullptr;), только после делать (new).
    */
    deleteModels();
    ///Тут должен быть код ДЗ
    QString request =
            "SELECT title, description FROM " + tableName_str + " f\
            JOIN film_category fc on f.film_id = fc.film_id\
            JOIN category c on c.category_id = fc.category_id WHERE c.name = ";

    QString err;
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
            // QSqlTableModel: можно создать только после полного создания объекта
            // базы (QSqlDatabase) т.е. после подключения драйверов и имени базы
            model = new QSqlTableModel(0, *db);
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
            err = model->lastError().text();
            modelVariant->setValue(model);
            break;
        }

        case requestComedy:
        {
            //qDebug() << "Получаю комедии";
            qModel = new QSqlQueryModel();
            request += "'Comedy'";
            qModel->setQuery(request, *db);
            qModel->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
            qModel->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
            qModel->sort(1, Qt::AscendingOrder);

            err = qModel->lastError().text();
            modelVariant->setValue(qModel);
            break;
        }

        case requestHorrors:
        {
            //qDebug() << "Получаю ужасы";
            qModel = new QSqlQueryModel();
            request += "'Horror'";
            qModel->setQuery(request, *db);
            qModel->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
            qModel->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
            qModel->sort(1, Qt::AscendingOrder);

            err = qModel->lastError().text();
            modelVariant->setValue(qModel);
            break;
        }

        default:
        break;
    }
    emit sig_SendStatusRequest(err);
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return db->lastError();
}

QVariant *DataBase::getVarModel()
{
    return modelVariant;
}
