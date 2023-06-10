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
    // Отобразим диалоговое окно.
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

       auto conDb = [&]{ dataBase->ConnectToDataBase(connectData->getData()); };
       auto runConnect = QtConcurrent::run(conDb);
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
 */
void MainWindow::on_pb_request_clicked()
{
    /*
     * ВОПРОС 1:
     * -----------------------------------------------------------
     * Сейчас запрос к БД делается в том же потоке, работает без
     * проблем, все быстро и четко, хотя конечно надо бы отправить
     * в другой поток, в случае проблем с БД -> программа НЕ зависнет.
     *
     * Но, если закоментить строчку и раскоментить две ниже, программа
     * тоже работает, но в "3 Вывод приложения" при каждом запросе к БД
     * ругается!
    */
    dataBase->RequestToDB(ui->cb_category->currentIndex());
    //auto reqDb = [&]{ dataBase->RequestToDB(ui->cb_category->currentIndex()); };
    //auto runRequest = QtConcurrent::run(reqDb);
}

/*!
 * \brief Обработчик кнопки "Очистить"
 */
void MainWindow::on_pb_clear_clicked()
{
    ui->tableView->setModel(0);
    ui->pb_clear->setEnabled(false);
}

/*!
 * \brief Слот отображает значение в QTableView
 * \param tabView
 * \param typeRequest
 */
void MainWindow::ScreenDataFromDB(const QVariant* model)
{
    ui->pb_clear->setEnabled(true);
    ui->tableView->setModel(0);

    /*
     * ВОПРОС 2:
     * ------------------------------------------------------
     * Видимо на формирование таблицы (особенно ВСЕХ фильмов)
     * программа заметно "призадумывается", как бы поместить
     * весь этот код в другой поток?
     * Думаю тормозит он в этот момент: ui->tableView->setModel...
     * Как я понимаю - этот код не может выполняться в потоке
     * отличном от потока родителя (ui)? Ну может можно его
     * выполнить в другом потоке, а затем объект вернуть в
     * этот поток? Или может я вообще не туда думаю?
     *
     * Я перепробовал 1000 вариантов, но не разу не угодал))
     * программа либо падала при запросе, либо так же сыпала
     * предупреждениями, (что то про потоки) как в вопросе 1.
    */

    switch (ui->cb_category->currentIndex() + 1)
    {
        case requestAllFilms:
        // Устанавливаем модель на TableView
        ui->tableView->setModel(model->value<QSqlTableModel*>());
        ui->tableView->hideColumn(0);               // Скрываем колонку (0) с id

        model->value<QSqlTableModel*>()->select(); // Делаем выборку данных из таблицы
        break;

        case requestHorrors:
        case requestComedy:
        ui->tableView->setModel(model->value<QSqlQueryModel*>());
        break;

        default:
        break;
    }
    // Разрешаем выделение строк
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
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

        QMessageBox::critical(0, tr("Ошибка подключения к БД!"),
                              QString("База данных \"%1\" - НЕ подключена!\n\n%2")
                              .arg(connectData->getData().at(dbName), dataBase->GetLastError().text()),
                              QMessageBox::StandardButton::Close);
    }
}

/*!
 * \brief Метод обрабатывает ответ БД на поступивший запрос
 * \param err
 */
void MainWindow::ReceiveStatusRequestToDB(const QString& err)
{
    if(err != "")
    {
        QMessageBox::critical(0, tr("Ошибка запроса к БД!"), err,
                              QMessageBox::StandardButton::Close);
    }
    else ScreenDataFromDB(dataBase->getVarModel());
}
