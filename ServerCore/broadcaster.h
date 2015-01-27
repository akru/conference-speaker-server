#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QUdpSocket>
#include <QJsonDocument>
#include <server_information.h>
#include <app_information.h>
#include <voting_invite.h>

class Broadcaster : public QObject
{
    Q_OBJECT
public:
    explicit Broadcaster(QObject *parent = 0);
    // Every time the message send
    static const uint    timeInterval = 1000;
    static const quint16 discoverPort = 35000;

public slots:
    void setServerInformation(ServerInformation info);

    void setVotingInvite(VotingInvite invite);
    void unsetVotingInvite();

    void setAppInfo(AppInformation info);

private slots:
    void sendInformation();

private:
    QList<QHostAddress> addreses;
    QUdpSocket          sock;
    QByteArray          packetCache;
    QJsonObject         message;
};

#endif // BROADCASTER_H
