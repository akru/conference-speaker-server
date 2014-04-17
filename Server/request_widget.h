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
    explicit RequestWidget(UserInformation &user, Connection *client, QWidget *parent = 0);
    ~RequestWidget();

signals:
    void accepted(Connection *);
    void discarded(Connection *);

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
    Connection *client;
};

#endif // REQUEST_WIDGET_H
