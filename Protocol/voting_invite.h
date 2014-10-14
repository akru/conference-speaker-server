#ifndef VOTING_INVITE_H
#define VOTING_INVITE_H

#include <QJsonObject>
#include <QStringList>
#include <QUuid>

struct VotingInvite
{
    enum Mode {
        Simple,
        Custom
    };

    // Constructor by params
    VotingInvite(QString     question = "",
                 Mode        mode     = Simple,
                 QStringList answers  = QStringList(),
                 QUuid       uuid     = QUuid::createUuid())
        : uuid(uuid),
          question(question),
          mode(mode),
          answers(answers)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static VotingInvite fromJson(const QJsonObject &json);

    QUuid       uuid;
    QString     question;
    Mode        mode;
    QStringList answers;
};


#endif // VOTING_INVITE_H
