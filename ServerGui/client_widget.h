#ifndef CLIENT_WIDGET_H
#define CLIENT_WIDGET_H

#include <QWidget>
#include <user_information.h>

namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(UserInformation &user,
                          QString &address,
                          QWidget *parent = 0);
    ~ClientWidget();

signals:
    void banned(QString);

private slots:
    void on_cancelButton_clicked()
    {
        emit banned(address);
    }

private:
    Ui::ClientWidget *ui;
    QString address;
};

#endif // CLIENT_WIDGET_H
