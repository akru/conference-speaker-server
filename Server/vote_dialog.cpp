#include "vote_dialog.h"
#include "ui_vote_dialog.h"

VoteDialog::VoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VoteDialog)
{
    ui->setupUi(this);
}

VoteDialog::~VoteDialog()
{
    delete ui;
}

void VoteDialog::newVote(int userCount)
{
    voters.clear();
    ui->lcdNo->display(0);
    ui->lcdYes->display(0);
    ui->lcdOther->display(userCount);
}

void VoteDialog::vote(Connection *client, bool type)
{
    Q_ASSERT(client);

    if (voters.contains(client->getAddress()))
        // Emit denied signal when double vote
        emit voteDenied(client);
    else
    {
        // Append voter to list
        voters.append(client->getAddress());
        // Increment counter value
        if (type)
            appendYes();
        else
            appendNo();
        // Emit accepted signal
        emit voteAccepted(client);
    }
}

void VoteDialog::appendYes()
{
    if (ui->lcdOther->value())
    {
        ui->lcdYes->display(ui->lcdYes->value() + 1);
        ui->lcdOther->display(ui->lcdOther->value() - 1);
    }
}

void VoteDialog::appendNo()
{
    if (ui->lcdOther->value())
    {
        ui->lcdNo->display(ui->lcdNo->value() + 1);
        ui->lcdOther->display(ui->lcdOther->value() - 1);
    }
}

void VoteDialog::on_voteButton_clicked()
{
    newVote(ui->lcdNo->value()  +
            ui->lcdYes->value() +
            ui->lcdOther->value());
}
