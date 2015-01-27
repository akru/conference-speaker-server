#include "user_adapter.h"

UserAdapter::UserAdapter(User *user, QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ChannelAction>("ChannelAction");
    connect(this, SIGNAL(channelAcion(ChannelAction)),
            user, SLOT(channelAction(ChannelAction)));
}
