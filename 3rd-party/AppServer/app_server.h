#ifndef APP_SERVER_H
#define APP_SERVER_H

#include "appserver_global.h"
#include <QThread>
#include <QMap>

struct mg_server;
typedef QMap<QString, QByteArray> URIMap;

class APPSERVERSHARED_EXPORT AppServer : public QThread
{
    Q_OBJECT
public:
    AppServer(QObject *parent = 0);
    ~AppServer();

    void run();
    void addRouteFile(const QString &route, const QString &resname);
    void addRouteData(const QString &route, const QByteArray &res);

private:
    mg_server *server;
};

#endif // APP_SERVER_H
