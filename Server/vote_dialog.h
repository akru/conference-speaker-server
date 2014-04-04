#ifndef VOTE_DIALOG_H
#define VOTE_DIALOG_H

#include <QDialog>
#include <QList>
#include "connection.h"

namespace Ui {
class VoteDialog;
}

class VoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VoteDialog(QWidget *parent = 0);
    ~VoteDialog();

signals:
    void voteDenied(Connection *client);
    void voteAccepted(Connection *client);

public slots:
    void newVote(int userCount);
    void vote(Connection *client, bool type);

private slots:
    void on_voteButton_clicked();

private:
    void appendYes();
    void appendNo();

private:
    Ui::VoteDialog *ui;
    QList<QString> voters;
};

#endif // VOTE_DIALOG_H
