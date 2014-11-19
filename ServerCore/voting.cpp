#include "voting.h"

Voting::Voting(const VotingInvite &invite,
               QObject *parent)
    : QObject(parent)
{
    results.invite = invite;

    switch (invite.mode) {
    case VotingInvite::Simple:
        results.values.append(0);
        results.values.append(0);
        break;
    case VotingInvite::Custom:
        foreach (const QString &a, invite.answers) {
            Q_UNUSED(a)
            results.values.append(0);
        }
        break;
    }
}

Voting::~Voting()
{
    emit resultsUpdated(results);
}

void Voting::vote(QString address, QJsonObject request)
{
    if (voters.contains(address))
    {
        emit denied(address, "try to double vote");
        return;
    }

    QJsonValue uuid = request["uuid"];
    if (uuid.isUndefined() || QUuid(uuid.toString()) != results.invite.uuid)
    {
        emit denied(address, "mismatch vote id");
        return;
    }

    QJsonValue answer = request["answer"];
    if (answer.isUndefined())
    {
        emit denied(address, "answer does not exist");
        return;
    }

    switch (results.invite.mode) {
    case VotingInvite::Simple:
        if (!answer.isBool())
        {
            emit denied(address, "answer does not bool");
            return;
        }
        if (answer.toBool())
            results.values[0] += 1;
        else
            results.values[1] += 1;
        break;
    case VotingInvite::Custom:
        if (!answer.isDouble())
        {
            emit denied(address, "answer does not integer");
            return;
        }
        results.values[answer.toInt()] += 1;
        break;
    }
    voters.append(address);
    emit accepted(address);
    emit resultsUpdated(results);
}