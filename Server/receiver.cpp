#include "receiver.h"


Receiver::Receiver(QObject *parent)
    : QObject(parent)
{
    // Test
    channel = ChannelInformation(1000);
}
