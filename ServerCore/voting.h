#ifndef VOTING_H
#define VOTING_H

#include "user.h"
#include <voting_invite.h>

#include <QUuid>
#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include <QVector>

struct VoteResults
{
    VotingInvite  invite;
    /*
     * Mode has to cases for results:
     *   Simple - values has two elements: Yes and No
     *   Custom - values has as long elements as answers in a question.
     * Values of elements is a count of clients with this answer.
     */
    QVector<uint> values;
};

class Voting : public QObject
{
    Q_OBJECT
public:
    explicit Voting(const VotingInvite &invite,
                    QObject *parent = 0);
    ~Voting();

    /*
     * Voting method,
     * inputs: user id and request
     * returns: response
     */
    QJsonObject vote(QString id, QJsonObject request);

signals:
    void resultsUpdated(VoteResults);

private:
    QList<QString> voters;
    VoteResults     results;
};

#endif // VOTING_H
