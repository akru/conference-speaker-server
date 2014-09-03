#ifndef REQUEST_WIDGET_H
#define REQUEST_WIDGET_H

#include <QWidget>
#include <user_information.h>
#include "connection.h"

namespace Ui {
class RequestWidget;
}

class RequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RequestWidget(UserInformation &user,
                           QString address,
                           QWidget *parent = 0);
    ~RequestWidget();

signals:
    void accepted(QString address);
    void discarded(QString address);

private slots:
    void on_okButton_clicked()
    {
        emit accepted(client);
    }
    void on_cancelButton_clicked()
    {
        emit discarded(client);
    }

private:
    Ui::RequestWidget *ui;
    QString            client;
};

#endif // REQUEST_WIDGET_H
