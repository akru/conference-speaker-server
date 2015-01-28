#include "app_server.h"
#include "user_adapter.h"
#include "mongoose.h"

#include <gate.h>
#include <user.h>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QTime>

#include <QDebug>

static const char *header_1 = "Загрузите приложение для вашей платформы:";
static const char *header_2 = "Нам важно, чтобы вы были услышаны!";

static URIMap     storage;
static char       ha1[33];

static AppServer  *self = 0;
static QList<User*> users;
static QList<User*> requests;

static int static_handler(struct mg_connection *conn) {
//    qDebug() << "STATIC_APP_SERVER" << QTime::currentTime().toString()
//             << "incoming request:" << conn->uri;
    foreach (const QString &uri, storage.keys()) {
        if (QString::fromLatin1(conn->uri) == uri) {
            QByteArray content = storage[uri];
            mg_send_data(conn, content.data(), content.size());
            return MG_REQUEST_PROCESSED;   // Mark as processed
        }
    }
    //qDebug() << "Not found";
    return MG_REQUEST_NOT_PROCESSED;
}

static int auth_handler(struct mg_connection *conn) {
    return mg_authorize_digest(conn, "admin", ha1);
}

static int remote_admin_handler(struct mg_connection *conn)
{
    QString content = QString::fromLatin1(conn->content, conn->content_len);
    QString request = QString::fromLatin1(conn->uri);
//    qDebug() << "REMOTE_ADMIN_SERVER" << QTime::currentTime().toString()
//             << "incoming request:" << conn->uri
//             << "content:" << content;
    // Check static data
    if (static_handler(conn) == MG_REQUEST_PROCESSED)
        return MG_REQUEST_PROCESSED;
    // Dynamic mgt
    if (request == QStringLiteral("/list"))
    {
        QJsonObject msg;
        QJsonObject userMap;
        QJsonArray channels;
        foreach (const User *u, users) {
            userMap.insert(u->getAddress(), u->getInfo().toJson());
            if (u->getState() == User::Speak)
                channels.append(u->getAddress());
        }
        msg.insert("users", userMap);
        msg.insert("channels", channels);

        QJsonArray reqs;
        foreach (const User *u, requests) {
            reqs.append(u->getAddress());
        }
        msg.insert("requests", reqs);

        QByteArray packet = QJsonDocument(msg).toJson();
        mg_send_data(conn, packet.data(), packet.size());
        return MG_REQUEST_PROCESSED;

    } else if (request == QStringLiteral("/accept"))
    {
        self->channelOpen(content);
        mg_printf(conn, "OK");
        return MG_REQUEST_PROCESSED;
    } else if (request == QStringLiteral("/close"))
    {
        self->channelClose(content);
        mg_printf(conn, "OK");
        return MG_REQUEST_PROCESSED;
    }
    return MG_REQUEST_NOT_PROCESSED;
}

AppServer::AppServer(const QString &adminPassword)
    : QThread(0),
      staticServer(mg_create_server(NULL)),
      remoteAdminServer(mg_create_server(NULL))
{
    setAdminPassword(adminPassword);

    mg_set_option(remoteAdminServer, "listening_port", "35081");
    mg_set_auth_handler(remoteAdminServer, auth_handler);
    mg_set_request_handler(remoteAdminServer, remote_admin_handler);

    mg_set_option(staticServer, "listening_port", "35080");
    mg_set_request_handler(staticServer, static_handler);
    // Load storage to memory
    QFile ix(":/app-server/index.html"); ix.open(QIODevice::ReadOnly);
    addRouteData("/", QString::fromLatin1(ix.readAll()).arg(tr(header_1)).arg(tr(header_2)).toUtf8());
    addRouteFile("/logo.png", ":/app-server/logo.png");
    addRouteFile("/apple.png", ":/app-server/apple.png");
    addRouteFile("/android.png", ":/app-server/android.png");
    addRouteFile("/wtlogo.png", ":/app-server/wtlogo.png");
    addRouteFile("/app/android/cs.apk", ":/app-server/cs.apk");

    addRouteFile("/admin", ":/app-server/admin.html");
    addRouteFile("/bootstrap.min.css", ":/app-server/bootstrap.min.css");
    addRouteFile("/bootstrap-theme.min.css", ":/app-server/bootstrap-theme.min.css");
    addRouteFile("/bootstrap.min.js", ":/app-server/bootstrap.min.js");
    addRouteFile("/jquery.min.js", ":/app-server/jquery.min.js");

    start(QThread::LowestPriority);
}

AppServer::~AppServer()
{
    terminate();
    wait();
    mg_destroy_server(&staticServer);
    mg_destroy_server(&remoteAdminServer);
}

AppServer * AppServer::instance()
{
    if (!self)
        self = new AppServer();
    return self;
}

void AppServer::setAdminPassword(const QString &password)
{
    // Calc HA1 by username, realm and password
    char _ha1[100];
    sprintf(_ha1, "admin:Conference Speaker:%s",
            password.toStdString().data());
    mg_md5(ha1, _ha1, NULL);
}

void AppServer::connectGate(Gate *gate)
{
    users.clear(); requests.clear();
    connect(gate, SIGNAL(connected(User*)), SLOT(appendUser(User*)));
}

void AppServer::appendUser(User *user)
{
    users.append(user);
    connect(user, SIGNAL(disconnected()), SLOT(removeUser()));
    connect(user, SIGNAL(requestChannelOpen()), SLOT(appendRequest()));
    connect(user, SIGNAL(channelOpened()), SLOT(remoteRequest()));
    connect(user, SIGNAL(channelClosed()), SLOT(remoteRequest()));
}

void AppServer::removeUser()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    users.removeOne(user);
}

void AppServer::appendRequest()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    if (!requests.contains(user))
        requests.append(user);
}

void AppServer::remoteRequest()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    if (requests.contains(user))
        requests.removeOne(user);
}

void AppServer::channelOpen(QString id)
{
    foreach (User *u, users) {
        if(u->getAddress() == id)
        {
            UserAdapter(u).openChannel();
            break;
        }
    }
}

void AppServer::channelClose(QString id)
{
    foreach (User *u, users) {
        if(u->getAddress() == id)
        {
            // TODO: Too dangerous! >:-|
            if (requests.contains(u))
                UserAdapter(u).denyChannel();
            else
                UserAdapter(u).closeChannel();
            break;
        }
    }
}

void AppServer::run()
{
    qDebug() << "AppServer started";
    for (;;)
    {
        mg_poll_server(staticServer, 100);  // Infinite loop, Ctrl-C to stop
        mg_poll_server(remoteAdminServer, 100);  // Infinite loop, Ctrl-C to stop
    }
}

void AppServer::addRouteFile(const QString &route, const QString &resname)
{
    QFile res(resname);
    res.open(QIODevice::ReadOnly);
    storage.insert(route, res.readAll());
}

void AppServer::addRouteData(const QString &route, const QByteArray &res)
{
    storage.insert(route, res);
}
