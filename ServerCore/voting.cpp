#include "voting.h"
#include <QDebug>

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
        results.values.fill(0, invite.answers.size());
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
        qDebug() << "Vote denied from" << address << "double vote";
        emit denied(address, "try to double vote");
        return;
    }

    QJsonValue uuid = request["uuid"];
    if (uuid.isUndefined() || QUuid(uuid.toString()) != results.invite.uuid)
    {
        qDebug() << "Vote denied from" << address << "mismatch uuid";
        emit denied(address, "mismatch vote id");
        return;
    }

    QJsonValue answer = request["answer"];
    if (answer.isUndefined())
    {
        qDebug() << "Vote denied from" << address << "unexist answer";
        emit denied(address, "answer does not exist");
        return;
    }

    switch (results.invite.mode) {
    case VotingInvite::Simple:
        if (!answer.isBool())
        {
            qDebug() << "Vote denied from" << address << "broken bool answer";
            emit denied(address, "answer does not bool");
            return;
        }
        if (answer.toBool())
            results.values[0] += 1;
        else
            results.values[1] += 1;
        break;
    case VotingInvite::Custom:
        if (!answer.isDouble() || answer.toInt() >= results.values.size())
        {
            qDebug() << "Vote denied from" << address << "broken answer";
            emit denied(address, "answer does not integer");
            return;
        }
        results.values[answer.toInt()] += 1;
        break;
    }
    voters.append(address);
    emit accepted(address);
    emit resultsUpdated(results);
    qDebug() << "Vote accepted from" << address;
}
