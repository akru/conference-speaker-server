#include "voting_invite.h"
#include <QJsonArray>
#include <QVariant>
#include "cs_packet.h"

// Json serializer
QJsonObject VotingInvite::toJson() const
{
    QJsonObject obj;
    obj.insert("uuid", uuid.toString());
    obj.insert("question", question);
    switch (mode) {
    case Simple:
        obj.insert("mode", "simple");
        break;
    case Custom:
        obj.insert("mode", "custom");
        obj.insert("answers", QJsonArray::fromStringList(answers));
    }
    return obj;
}

// Json deserializer
VotingInvite VotingInvite::fromJson(const QJsonObject &json)
{
    QJsonValue uuid    = json["uuid"];
    QJsonValue question= json["question"];
    QJsonValue mode    = json["mode"];
    QJsonValue answers = json["answers"];

    if (uuid.isUndefined() || question.isUndefined() || mode.isUndefined())
        throw(BadPacket());

    if (mode.toString() == "simple")
        return VotingInvite(question.toString(),
                            Simple,
                            QStringList(),
                            QUuid(uuid.toString()));
    else
        return VotingInvite(question.toString(),
                            Custom,
                            answers.toVariant().toStringList(),
                            QUuid(uuid.toString()));
}
