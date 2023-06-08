#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>
#include <QSqlTableModel>
#include <QFuture>
#include "database.h"
#include "dbdata.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ScreenDataFromDB(const QVariant *model);
    void ReceiveStatusConnectionToDB(bool status);
    void ReceiveStatusRequestToDB(QSqlError err);

private slots:
    void on_act_addData_triggered();
    void on_act_connect_triggered();
    void on_pb_request_clicked();
    void on_pb_clear_clicked();

signals:
    //void sig_RequestToDb(QString request);

private:
    Ui::MainWindow* ui;
    DbData* connectData;
    DataBase* dataBase;
};
#endif // MAINWINDOW_H
