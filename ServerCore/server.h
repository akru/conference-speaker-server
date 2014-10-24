#ifndef SERVER_H
#define SERVER_H

#include <QMap>
#include <QObject>
#include <user_information.h>
#include <server_information.h>
#include <voting_invite.h>
#include "voting.h"

#ifndef QT_DEBUG
#include "licensing.h"
#endif

class QTcpServer;
class Connection;
class Broadcaster;
class Receiver;
class Recorder;
class Speaker;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(const ServerInformation &info, QObject *parent = 0);
    ~Server();

signals:
    void userConnected(QString address, UserInformation info);
    void userDisconnected(QString address);
    void userRegistrationRequest(QString address, UserInformation info);

    void channelConnected(QString address);
    void channelDisconnected(QString address);
    void channelRequest(QString address, UserInformation info);
    void channelCloseRequest(QString address);
    void channelVolumeChanged(QString address, qreal volume);
    void channelVolumeChanged(qreal volume);
    void channelAmpUpdated(QString address, ushort amp);
    void channelSettingsUpdated();

    void voteRequest(QString address, QJsonObject request);
    void voteResultsUpdated(VoteResults results);

    void recordStarted();
    void recordStoped();

public slots:
    void userDrop(QString address);

    void channelDeny(QString address);
    void channelOpen(QString address);
    void channelClose(QString address);
    void channelVolume(QString address, qreal volume);
    void channelVolume(qreal volume);
    void channelAmp(QString address, ushort amp);
    void channelReloadSettings();

    void voteNew(VotingInvite invite);
    void voteStop();

    void recordStart();
    void recordStop();

private slots:
    void connectionNew();
    void connectionReadyRead(Connection *client);
    void connectionClose(Connection *client);

    void userRegister(QString address, UserInformation info);

    void voteAccept(QString address);
    void voteDeny(QString address, QString error);
    void voteReadyResults(VoteResults results);

private:
    // TCP server instance
    QTcpServer                    *server;
    // Broadcaster instance
    Broadcaster                   *broadcaster;
    // Voting instance
    Voting                        *voting;
    // Speaker instance
    Speaker                       *speaker;
    // Speaker recorder
    Recorder                      *recorder;
    // User information by address map
    QMap<QString, UserInformation> users;
    // Channel information by address map
    QMap<QString, Receiver *>      channels;
    // Connection by address map
    QMap<QString, Connection *>    clients;
#ifndef QT_DEBUG
    // Licensing policy
    Licensing                      license;
#endif
};

#endif // SERVER_H
