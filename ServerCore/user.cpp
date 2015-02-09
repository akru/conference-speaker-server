#include "user.h"
#include "receiver.h"
#include "speaker.h"

#include <request.h>
#include <registration_request.h>

#include <response.h>
#include <channel_response.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include <QTimer>
#include <QDebug>

User::User(QTcpSocket *connection, QObject *parent)
    : QObject(parent),
      sock(connection),
      userState(Unregistered),
      receiver(0)
{
    connect(sock, SIGNAL(readyRead()),    SLOT(readMessage()));
    connect(sock, SIGNAL(disconnected()), SLOT(sockDisconnected()));

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SLOT(timeout()));
    t->setInterval(1000);
    t->start();
}

void User::timeout()
{
    ss = sock->state();
}

QString User::getAddress() const
{
    return sock->peerAddress().toString();
}

void User::registration(UserInformation info)
{
    Response res;
    if (userState == Unregistered)
    {
        userInfo = info;
        userState = Registered;
        qDebug() << "USER" << getAddress() << "::"
                 << "Registered.";
        res = Response(Request::Registration, Response::Success);
    }
    else
    {
        qWarning() << "USER" << getAddress() << "::"
                   << "Try to double registration.";
        res = Response(Request::Registration,
                       Response::Error, "User already registered");
    }
    messageSend(res.toJson());
}

void User::channelAction(ChannelAction action)
{
    QJsonObject res;
    // Registration check
    if (userState == Unregistered)
    {
        qWarning() << "USER" << getAddress() << "::"
                   << "Try to channelAction without registration!";
        Response response(Request::ChannelOpen,
                          Response::Error, "Please register first");
        messageSend(response.toJson());
        return;
    }
    // Switch by action
    switch (action) {
    case ChOpen:
        // Exist channel check
        if (userState != Speak)
        {
            try {
                receiver = new Receiver(sock->localAddress(), this);
                // Connect receiver with speaker
                connect(receiver,            SIGNAL(sampleReceived(User*,QByteArray)),
                        Speaker::instance(), SLOT(incomingData(User*,QByteArray)));
                userState = Speak;
            } catch (...) {
                qWarning() << "USER" << getAddress() << "::"
                           << "Unable to open channel!";
                res = Response(Request::ChannelOpen,
                               Response::Error, "Server fault").toJson();
                break;
            }
        }
        res = ChannelResponse(receiver->getChannelInfo()).toJson();
        qDebug() << "USER" << getAddress() << "::"
                 << "Channel opened:" << receiver->getChannelInfo().toJson();
        emit channelOpened();
        break;

    case ChClose:
        // Exist channel check
        if (userState == Speak)
        {
            delete receiver;
            userState = Registered;
            qDebug() << "USER" << getAddress() << "::"
                     << "Channel close success.";
            res = Response(Request::ChannelClose, Response::Success).toJson();
        }
        else
        {
            qWarning() << "USER" << getAddress() << "::"
                       << "Try to close unopened channel!";
            res = Response(Request::ChannelClose,
                           Response::Error, "Channel is not open").toJson();
        }
        emit channelClosed();
        break;

    case ChDeny:
        qDebug() << "USER" << getAddress() << "::"
                 << "Channel open request denied.";
        res = Response(Request::ChannelOpen,
                       Response::Error, "Channel open request denied").toJson();
        emit channelClosed();
        break;
    }
    messageSend(res);
}

void User::voteResponse(QJsonObject response)
{
    qDebug() << "USER" << getAddress() << "::"
             << "Vote response:" << response;
    messageSend(response);
}

void User::readMessage()
{
    QJsonParseError err;
    QJsonDocument message = QJsonDocument::fromJson(sock->readAll(), &err);
    // Perse error check
    if (err.error != QJsonParseError::NoError)
    {
        qWarning() << "USER" << getAddress() << "::"
                   << "Message parse error:" << err.errorString();
        return;
    }
    try {
        messageParser(message.object());
    } catch (std::exception &e) {
        qWarning() << "USER" << getAddress() << "::"
                   << "Message parse exception:" << e.what();
        return;
    }
}

void User::sockDisconnected()
{
    emit disconnected();
}

void User::messageParser(const QJsonObject &message)
{
    Request          req = Request::fromJson(message);
    switch (req.type)
    {
    case Request::Registration:
        qDebug() << "USER" << getAddress() << "::"
                 << "Registration request.";
        emit requestRegistration(
                    RegistrationRequest::fromJson(message).user);
        break;
    case Request::ChannelOpen:
        qDebug() << "USER" << getAddress() << "::"
                 << "Channel open request.";
        emit requestChannelOpen();
        break;
    case Request::ChannelClose:
        qDebug() << "USER" << getAddress() << "::"
                 << "Channel close request.";
        emit requestChannelClose();
        break;
    case Request::Vote:
        qDebug() << "USER" << getAddress() << "::"
                 << "Vote request:" << message;
        emit requestVote(message);
        break;
    default:
        qWarning() << "USER" << getAddress() << "::"
                   << "Unknown request:" << message;
    }
}

void User::messageSend(const QJsonObject &message)
{
    QByteArray data = QJsonDocument(message).toJson();
    sock->write(data);
}
