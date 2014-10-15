#ifndef VOTING_H
#define VOTING_H

#include <voting_invite.h>

#include <QUuid>
#include <QObject>
#include <QJsonObject>
#include <QStringList>

struct VoteResults
{
    VotingInvite invite;
    /*
     * Mode has to cases for results:
     *   Simple - values has two elements: Yes and No
     *   Custom - values has as long elements as answers in a question.
     * Values of elements is a count of clients with this answer.
     */
    QList<uint>  values;
};

class Voting : public QObject
{
    Q_OBJECT
public:
    explicit Voting(const VotingInvite &invite,
                    QObject *parent = 0);
    ~Voting();

signals:
    void accepted(QString address);
    void denied(QString address, QString error);
    void resultsUpdated(VoteResults results);

public slots:
    void vote(QString address, QJsonObject request);

private:
    QStringList  voters;
    VoteResults  results;
};

#endif // VOTING_H
