#ifndef VOTE_WIDGET_H
#define VOTE_WIDGET_H

#include <QWidget>
#include <QList>
#include "connection.h"

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
    void voteDenied(Connection *client);
    void voteAccepted(Connection *client);

public slots:
    void appendClient();
    void dropClient();
    void vote(Connection *client, bool type);

private slots:
    void setCount(int count);
    void updateYesL(int count);
    void updateNoL(int count);
    void on_voteButton_clicked();

private:
    void appendYes();
    void appendNo();

private:
    Ui::VoteWidget *ui;
    QList<QString> voters;
    int count;
};

#endif // VOTE_WIDGET_H
