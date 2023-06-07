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
    connectData.resize(NUM_DATA_FOR_CONNECT_TO_DB);
    /*
        Имя хоста: 981757-ca08998.tmweb.ru
        Имя БД: netology_cpp
        Порт: 5432
        Логин: netology_usr_cpp
        Пароль: CppNeto3
    */
    connectData[hostName] = "981757-ca08998.tmweb.ru";
    connectData[dbName] = "netology_cpp";
    connectData[login] = "netology_usr_cpp";
    connectData[pass] = "CppNeto3";
    connectData[port] = "5432";

    dataToGUI();
}

DbData::~DbData()
{
    delete ui;
}

QVector<QString> DbData::getData() const
{
    return connectData;
}

/*!
 * \brief Обработчик кнопки "Ок" в диалоговом окне
 */
void DbData::on_buttonBox_accepted()
{
    //Добавляем данные в контейнер
    guiToData();
}

void DbData::dataToGUI()
{
    ui->le_host->setText(connectData[hostName]);
    ui->le_dbName->setText(connectData[dbName]);
    ui->le_login->setText(connectData[login]);
    ui->le_pass->setText(connectData[pass]);
    ui->spB_port->setValue(connectData[port].toInt());
}

void DbData::guiToData()
{
    connectData[hostName] = ui->le_host->text();
    connectData[dbName] = ui->le_dbName->text();
    connectData[login] = ui->le_login->text();
    connectData[pass] = ui->le_pass->text();
    connectData[port] = ui->spB_port->text();
}

