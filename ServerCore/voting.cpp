#include "voting.h"

#include <response.h>
#include <request.h>
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

QJsonObject Voting::vote(User::ID id, QJsonObject request)
{
    if (voters.contains(id))
    {
        qDebug() << "VOTE :: denied for" << id.show() << ", double vote";
        return Response(Request::Vote,
                        Response::Error, "Try to double vote").toJson();
    }

    QJsonValue uuid = request["uuid"];
    if (uuid.isUndefined() || QUuid(uuid.toString()) != results.invite.uuid)
    {
        qDebug() << "Vote denied for" << id.show() << "mismatch uuid";
        return Response(Request::Vote,
                        Response::Error, "Mismatch vote uuid").toJson();
    }

    QJsonValue answer = request["answer"];
    if (answer.isUndefined())
    {
        qDebug() << "Vote denied for" << id.show() << "unexist answer";
        return Response(Request::Vote,
                        Response::Error, "Answer does not exist").toJson();
    }

    switch (results.invite.mode) {
    case VotingInvite::Simple:
        if (!answer.isBool())
        {
            qDebug() << "Vote denied for" << id.show() << "broken bool answer";
            return Response(Request::Vote,
                            Response::Error, "Answer does not bool").toJson();
        }
        if (answer.toBool())
            results.values[0] += 1;
        else
            results.values[1] += 1;
        break;
    case VotingInvite::Custom:
        if (!answer.isDouble() || answer.toInt() < 0 || answer.toInt() >= results.values.size())
        {
            qDebug() << "VOTE :: denied for" << id.show() << "broken answer";
            return Response(Request::Vote,
                            Response::Error, "Answer does not integer").toJson();
        }
        results.values[answer.toInt()] += 1;
        break;
    }
    voters.append(id);
    emit resultsUpdated(results);
    qDebug() << "VOTE :: accepted for" << id.show();
    return Response(Request::Vote, Response::Success).toJson();
}
