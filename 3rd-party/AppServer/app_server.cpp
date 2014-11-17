#include "app_server.h"
#include "mongoose.h"
#include <QFile>

#include <QTime>
#include <QDebug>

static const char *header_1 = "Загрузите приложение для вашей платформы:";
static const char *header_2 = "Нам важно, чтобы вы были услышаны!";

static URIMap storage;

static int event_handler(struct mg_connection *conn, enum mg_event ev) {
    if (ev == MG_REQUEST) {
        qDebug() << "APP_SERVER" << QTime::currentTime().toString()
                 << "incoming request:" << conn->uri;
        foreach (const QString &uri, storage.keys()) {
            if (QString::fromLatin1(conn->uri) == uri) {
                QByteArray content = storage[uri];
                mg_send_data(conn, content.data(), content.size());
                return MG_TRUE;   // Mark as processed
            }
        }
        qDebug() << "Not found";
        return MG_FALSE;  // Rest of the events are not processed
    } else {
        return MG_FALSE;  // Rest of the events are not processed
    }
}

AppServer::AppServer(QObject *parent)
    : QThread(parent),
      server(mg_create_server(NULL, event_handler))
{
    mg_set_option(server, "listening_port", "35080");
    // Load storage to memory
    QFile ix(":/app-server/index.html"); ix.open(QIODevice::ReadOnly);
    addRouteData("/", QString::fromLatin1(ix.readAll()).arg(header_1).arg(header_2).toUtf8());
    addRouteFile("/logo.png", ":/app-server/logo.png");
    addRouteFile("/apple.png", ":/app-server/apple.png");
    addRouteFile("/android.png", ":/app-server/android.png");
    addRouteFile("/wtlogo.png", ":/app-server/wtlogo.png");
    addRouteFile("/app/android/cs.apk", ":/app-server/cs.apk");
}

AppServer::~AppServer()
{
    terminate();
    wait();
    mg_destroy_server(&server);
}

void AppServer::run()
{
    for (;;) {
        mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
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
