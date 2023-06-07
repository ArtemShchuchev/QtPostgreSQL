#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);
    ui->pb_clear->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */
    connectData = new DbData(this); // данные о соединении с БД
    dataBase = new DataBase(this);

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    // Соединяем сигнал, который передает ответ от БД с методом, который отображает ответ в ПИ
    connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    // Сигнал для подключения к БД
    // сигнал о статусе сообщения, связываю со слотом изменения ПИ
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(dataBase, &DataBase::sig_SendStatusRequest, this, &MainWindow::ReceiveStatusRequestToDB);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 *
 * клик по всплывающему меню "Ввести данные"
 */
void MainWindow::on_act_addData_triggered()
{
    // Отобразим диалоговое окно. Какой метод нужно использовать?
    connectData->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 *
 * клик по всплывающему меню "Подключиться"
 */
void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */

    if(ui->lb_statusConnect->text() == "Отключено")
    {
       ui->lb_statusConnect->setText("Подключение");
       ui->lb_statusConnect->setStyleSheet("color : black");

       dataBase->ConnectToDataBase(connectData->getData());
    }
    else
    {
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }
}

/*!
 * \brief Обработчик кнопки "Получить"
 *
 * requestAllFilms = 1,
 * requestComedy   = 2,
 * requestHorrors  = 3
 */
void MainWindow::on_pb_request_clicked()
{
    /*
     * В случае если ответ получен, то мы его читаем
     * в противном случае выводим ошибку. Сейчас мы разберем
     * получение всех фильмов. А дома вы получите отдельно комедии
     * и ужасы
    */
    ///Тут должен быть код ДЗ
    auto req = [this](int reqIdx){dataBase->RequestToDB(reqIdx);};
    auto future = QtConcurrent::run(req, ui->cb_category->currentIndex());
    future.waitForFinished();

    setupModel("film", dataBase->getHeaders());
    showDataBase();
}

/*!
 * \brief Обработчик кнопки "Очистить"
 */
void MainWindow::on_pb_clear_clicked()
{
    ui->tableView->setModel(0);
    ui->pb_clear->setEnabled(false);
}


void MainWindow::setupModel(const QString& tableName, const QStringList& headers)
{
    /* Производим инициализацию модели представления данных
     * с установкой имени таблицы в базе данных, по которому
     * будет производится обращение в таблице
     * */

    model = new QSqlTableModel(this, dataBase->getMyDb());
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //model->setFilter(); // фильтрация WHERE

    // Устанавливаем названия колонок в таблице с сортировкой данных
    for (int i(0); i < model->columnCount(); ++i)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
        //qDebug() << "Haeder: " << headers[i];
    }
    // Устанавливаем сортировку по возрастанию данных по нулевой колонке
    model->setSort(0, Qt::AscendingOrder);
}

void MainWindow::showDataBase()
{
    ui->tableView->setModel(model);             // Устанавливаем модель на TableView
    ui->tableView->hideColumn(0);               // Скрываем колонку (0) с id
    // Разрешаем выделение строк
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    model->select(); // Делаем выборку данных из таблицы
    ui->pb_clear->setEnabled(true);
}

/*!
 * \brief Слот отображает значение в QTableView
 * \param tabView
 * \param typeRequest
 */
void MainWindow::ScreenDataFromDB(const QTableView *tabView, int typeRequest)
{

    ///Тут должен быть код ДЗ

    /*
     * По аналогии с формированием таблицы заполним таблицу
     * которая распаложена на главной форме. Необходимо пройтись
     * по всем элементам и приравнять их.
     * Также привяжем ширину заголовка к ширене окна.
    */
    switch (typeRequest)
    {
    case requestAllFilms:
    case requestHorrors:
    case requestComedy:
    {
/*
        ui->tb_result->setRowCount(widget->rowCount());
        ui->tb_result->setColumnCount(widget->columnCount());
        QStringList hdrs;
        for(int i = 0; i < widget->columnCount(); ++i)
        {
            hdrs << widget->horizontalHeaderItem(i)->text();
        }
        ui->tb_result->setHorizontalHeaderLabels(hdrs);

        for(int i = 0; i<widget->rowCount(); ++i)
        {
            for(int j = 0; j<widget->columnCount(); ++j)
            {
                ui->tb_result->setItem(i,j, widget->item(i,j)->clone());
            }
        }

        ui->tb_result->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
*/
        break;
    }
    default:
        break;
    }
}

/*!
 * \brief Метод изменяет соcтояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    /*
        Сделал попытку подключения и через сигнал передал
        сюда статус подключения.
        Меняю ПИ в соответствии со статусом подключения.
    */
    if(status)
    {
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else
    {

        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");

        QMessageBox::critical(0, tr("Ошибка подключения к БД!"), "База данных \""
                              + connectData->getData().at(dbName)
                              + "\" - НЕ подключена!\n\n"
                              + dataBase->GetLastError().text(),
                              QMessageBox::StandardButton::Close);
    }
}

/*!
 * \brief Метод обрабатывает ответ БД на поступивший запрос
 * \param err
 */
void MainWindow::ReceiveStatusRequestToDB(QSqlError* err)
{
    if(err->type() != QSqlError::NoError)
    {
        QMessageBox::critical(0, tr("Ошибка запроса к БД!"), err->text(),
                              QMessageBox::StandardButton::Close);
    }
    else dataBase->ReadAnswerFromDB(ui->cb_category->currentIndex());
}
