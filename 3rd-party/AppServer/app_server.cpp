#include "app_server.h"
#include "mongoose.h"
#include <QFile>

#include <QTime>
#include <QDebug>

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
    addRouteFile("/", ":/app-server/index.html");
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
