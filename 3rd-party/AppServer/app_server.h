#ifndef APP_SERVER_H
#define APP_SERVER_H

#include <QThread>
#include <QMap>
#include <user.h>

class Gate;
struct mg_server;
typedef QMap<QString, QByteArray> URIMap;

class AppServer : public QThread
{
    Q_OBJECT
    AppServer(const QString &adminPassword = "admin");
    ~AppServer();

public:
    static AppServer * instance();
    void run();

public slots:
    void addRouteFile(QString route, QString resname);
    void addRouteData(QString route, QByteArray res);

    void setAdminPassword(QString password);
    void connectGate(Gate *gate);

    void channelOpen(QString id);
    void channelClose(QString id);

private slots:
    void appendUser(User *user);
    void removeUser();
    void appendRequest();
    void removeRequest();

private:
    mg_server *staticServer;
    mg_server *remoteAdminServer;
};

#endif // APP_SERVER_H
