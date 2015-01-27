#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>

#include <user_information.h>
#include <channel_information.h>

class QHostAddress;
class QTcpSocket;
class Receiver;

// Avaliable channel actions
enum ChannelAction {
    ChOpen, ChClose, ChDeny
};

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QTcpSocket *connection,
                  QObject *parent = 0);
    // Uniq User ident
    class ID {
        const quint32 value;
    public:
        ID(const QString &address);
        ID(const QHostAddress &address);
        QString show() const;
        bool operator <(const ID &id) const;
        bool operator ==(const ID &id) const;
    };
    // Available User states
    enum State {
        Unregistered,
        Registered,
        Speak
    };
    // User ident getter
    inline const ID & getID() const
    { return userID; }
    // User state getter
    inline const State & getState() const
    { return userState; }
    // User information getter
    inline const UserInformation & getInfo() const
    { return userInfo; }
    // Channel information getter
    inline const ChannelInformation & getChannelInfo() const
    { return channelInfo; }

signals:
    void disconnected();

    void requestRegistration(UserInformation);
    void requestChannelOpen();
    void requestChannelClose();
    void requestVote(QJsonObject);

    void channelOpened();
    void channelClosed();

public slots:
    void registration(UserInformation info);
    void channelAction(ChannelAction action);
    void voteResponse(QJsonObject response);

private slots:
    void readMessage();
    void sockDisconnected();

private: // Private methods
    void messageParser(const QJsonObject &message);
    void messageSend(const QJsonObject &message);

private: // Private rows
    // TCP user connection
    QTcpSocket        *sock;
    // User ident
    ID                 userID;
    // User state
    State              userState;
    // User information
    UserInformation    userInfo;
    // Channel information
    ChannelInformation channelInfo;
    // Channel receiver
    Receiver          *receiver;
};

#endif // USER_H