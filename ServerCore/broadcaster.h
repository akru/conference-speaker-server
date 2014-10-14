#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QUdpSocket>
#include <QJsonDocument>
#include <server_information.h>
#include <voting_invite.h>

const int BROADCASTER_TIME_INTERVAL = 1000;

class Broadcaster : public QObject
{
    Q_OBJECT
public:
    explicit Broadcaster(QObject *parent = 0);

public slots:
    void setServerInformation(ServerInformation info);

    void setVotingInvite(VotingInvite invite);
    void unsetVotingInvite();

private slots:
    void sendInformation();

private:
    QList<QHostAddress> addreses;
    QUdpSocket          sock;
    QByteArray          packetCache;
    QJsonObject         message;
};

#endif // BROADCASTER_H
