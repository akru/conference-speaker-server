#include "app_server.h"
#include "mongoose.h"

#include <server.h>

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

struct ServerInfo {
    ServerInfo(const UserMap &u, const ChannelMap &c)
        : users(u), channels(c) {}
    const UserMap    &users;
    const ChannelMap &channels;
    QList<QString>   requests;
};

static AppServer  *instance;
static ServerInfo *srv;

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
        foreach (const QString &id, srv->users.keys()) {
            userMap.insert(id, srv->users[id].toJson());
        }
        msg.insert("users", userMap);
        msg.insert("channels", QJsonArray::fromStringList(srv->channels.keys()));
        msg.insert("requests", QJsonArray::fromStringList(srv->requests));
        QByteArray packet = QJsonDocument(msg).toJson();
        mg_send_data(conn, packet.data(), packet.size());
        return MG_REQUEST_PROCESSED;

    } else if (request == QStringLiteral("/accept"))
    {
        instance->channelOpen(content);
        mg_printf(conn, "OK");
        return MG_REQUEST_PROCESSED;
    } else if (request == QStringLiteral("/close"))
    {
        if (srv->requests.contains(content))
            instance->channelDeny(content);
        else
            instance->channelClose(content);
        mg_printf(conn, "OK");
        return MG_REQUEST_PROCESSED;
    }
    return MG_REQUEST_NOT_PROCESSED;
}

AppServer::AppServer(Server *parent,
                     const QString &adminPassword)
    : QThread(parent),
      staticServer(mg_create_server(NULL)),
      remoteAdminServer(mg_create_server(NULL))
{

    instance = this;
    // Calc HA1 by username, realm and password
    char _ha1[100];
    sprintf(_ha1, "admin:Conference Speaker:%s",
            adminPassword.toStdString().data());
    mg_md5(ha1, _ha1, NULL);

    srv = new ServerInfo(parent->getUsers(),
                         parent->getChannels());

    connect(this, SIGNAL(channelIsOpen(QString)),
            parent, SLOT(channelOpen(QString)));
    connect(this, SIGNAL(channelIsDeny(QString)),
            parent, SLOT(channelDeny(QString)));
    connect(this, SIGNAL(channelIsClose(QString)),
            parent, SLOT(channelClose(QString)));

    connect(parent,
            SIGNAL(channelRequest(QString)),
            SLOT(appendRequest(QString)));
    connect(parent,
            SIGNAL(channelConnected(QString)),
            SLOT(removeRequest(QString)));
    connect(parent,
            SIGNAL(channelCloseRequest(QString)),
            SLOT(removeRequest(QString)));
    connect(parent,
            SIGNAL(channelDisconnected(QString)),
            SLOT(removeRequest(QString)));
    connect(parent,
            SIGNAL(userDisconnected(QString)),
            SLOT(removeRequest(QString)));


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
}

AppServer::~AppServer()
{
    terminate();
    wait();
    mg_destroy_server(&staticServer);
    mg_destroy_server(&remoteAdminServer);
    delete srv;
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

void AppServer::appendRequest(QString address)
{
    srv->requests.append(address);
}

void AppServer::removeRequest(QString address)
{
    srv->requests.removeAll(address);
}
