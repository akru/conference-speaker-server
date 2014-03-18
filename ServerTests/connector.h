#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QTcpSocket>

#include <server_information.h>
#include <user_information.h>
#include <channel_information.h>

class Connector : public QObject
{
    Q_OBJECT
public:
    explicit Connector(QObject *parent = 0);

signals:
    void connected();
    void successRegistration();
    void successChannel(ChannelInformation);
    void failureRegistration(QString);
    void failureChannel(QString);

public slots:
    void setServer(ServerInformation server);
    void registerUser(UserInformation user);
    void startTransmit();

    void sockConnected()
    {
        UserInformation user("test user name");
        registerUser(user);
    }
    void sockReadyRead();

private:
    QTcpSocket sock;
};

#endif // CONNECTOR_H
