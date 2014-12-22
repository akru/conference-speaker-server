#ifndef APP_SERVER_H
#define APP_SERVER_H

#include "appserver_global.h"
#include <QThread>
#include <QMap>

class Server;
struct mg_server;
typedef QMap<QString, QByteArray> URIMap;

class APPSERVERSHARED_EXPORT AppServer : public QThread
{
    Q_OBJECT
public:
    AppServer(Server *parent,
              const QString &adminPassword = "admin");
    ~AppServer();

    void run();
    void addRouteFile(const QString &route, const QString &resname);
    void addRouteData(const QString &route, const QByteArray &res);

    void channelDeny(QString address)
    { emit channelIsDeny(address); }
    void channelOpen(QString address)
    { emit channelIsOpen(address); }
    void channelClose(QString address)
    { emit channelIsClose(address); }

signals:
    void channelIsDeny(QString address);
    void channelIsOpen(QString address);
    void channelIsClose(QString address);

private slots:
    void appendRequest(QString address);
    void removeRequest(QString address);

private:
    mg_server *staticServer;
    mg_server *remoteAdminServer;
};

#endif // APP_SERVER_H
