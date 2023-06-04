#include "dbdata.h"
#include "database.h"
#include "ui_dbdata.h"

DbData::DbData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbData)
{
    ui->setupUi(this);

    //Ресайзим вектор значений, по количеству полей необходимых для
    //подключения к БД
    data.resize(NUM_DATA_FOR_CONNECT_TO_DB);
    /*
        Имя хоста: 981757-ca08998.tmweb.ru
        Имя БД: netology_cpp
        Порт: 5432
        Логин: netology_usr_cpp
        Пароль: CppNeto3
    */
    data[hostName] = "981757-ca08998.tmweb.ru";
    data[dbName] = "netology_cpp";
    data[login] = "netology_usr_cpp";
    data[pass] = "CppNeto3";
    data[port] = "5432";

    dataToGUI();
}

DbData::~DbData()
{
    delete ui;
}

QVector<QString> DbData::getData()
{
    return data;
}

/*!
 * \brief Обработчик кнопки "Ок" в диалоговом окне
 */
void DbData::on_buttonBox_accepted()
{
    //Добавляем данные в контейнер
    guiToData();

    //emit sig_sendData(data);  // передаем в главное окно
}

void DbData::dataToGUI()
{
    ui->le_host->setText(data[hostName]);
    ui->le_dbName->setText(data[dbName]);
    ui->le_login->setText(data[login]);
    ui->le_pass->setText(data[pass]);
    ui->spB_port->setValue(data[port].toInt());
}

void DbData::guiToData()
{
    data[hostName] = ui->le_host->text();
    data[dbName] = ui->le_dbName->text();
    data[login] = ui->le_login->text();
    data[pass] = ui->le_pass->text();
    data[port] = ui->spB_port->text();
}

