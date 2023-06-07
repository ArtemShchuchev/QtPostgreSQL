#ifndef DBDATA_H
#define DBDATA_H

#include <QDialog>

namespace Ui {class DbData;}

/*
    Класс содержит инфо о подключении к БД (логин, пароль...)
*/
class DbData : public QDialog
{
    Q_OBJECT

public:
    explicit DbData(QWidget *parent = nullptr);
    ~DbData();
    QVector<QString> getData() const;

 signals:

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DbData *ui;
    QVector<QString> connectData;
    void dataToGUI();
    void guiToData();
};

#endif // DBDATA_H
