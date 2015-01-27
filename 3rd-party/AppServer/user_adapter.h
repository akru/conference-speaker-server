#ifndef USER_ADAPTER_H
#define USER_ADAPTER_H

#include <QObject>
#include <user.h>

class UserAdapter : public QObject
{
    Q_OBJECT
public:
    explicit UserAdapter(User *user, QObject *parent=0);

    void openChannel()
    { emit channelAcion(ChOpen); }
    void closeChannel()
    { emit channelAcion(ChClose); }
    void denyChannel()
    { emit channelAcion(ChDeny); }

signals:
    void channelAcion(ChannelAction);

};

#endif // USER_ADAPTER_H
