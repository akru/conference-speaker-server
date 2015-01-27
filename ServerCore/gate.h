#ifndef GATE_H
#define GATE_H

#include <QMap>
#include "user.h"
#ifndef QT_DEBUG
#include "licensing.h"
#endif

class QTcpServer;

class Gate : public QObject
{
    Q_OBJECT
public:
    explicit Gate(QObject *parent = 0,
                  const QString &listeningAddress = "127.0.0.1",
                  quint16 listeningPort = 35001);

    inline bool isEnabled() const
    { return listenEnabled; }

signals:
    void connected(User*);

private slots:
    void newConnection();
    void userDisconnected();

private:
    // TCP server instance
    QTcpServer      *server;
    // User by ID map
    QList<User::ID>  users;
#ifndef QT_DEBUG
    // Licensing policy
    Licensing    license;
#endif
    // Listening flag
    bool         listenEnabled;
};

#endif // GATE_H
