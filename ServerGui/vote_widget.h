#ifndef VOTE_WIDGET_H
#define VOTE_WIDGET_H

#include <QLabel>
#include <QWidget>
#include <QLineEdit>

#include <voting_invite.h>
#include <voting.h>

namespace Ui {
class VoteWidget;
}

class VoteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VoteWidget(QWidget *parent = 0);
    ~VoteWidget();

signals:
    void voteNew(VotingInvite invite);
    void voteStop();

public slots:
    void updateResults(VoteResults results);

private slots:
    void on_plusButton_clicked();
    void on_pushButton_toggled(bool checked);
    void enableCustom(bool enable);
    void on_pushButton_2_toggled(bool checked);

private:
    Ui::VoteWidget *ui;
    QList<QLineEdit*> answers;
    QList<QLabel *>   resultAnswers;
    QWidget          *resultWidget;
};

#endif // VOTE_WIDGET_H
