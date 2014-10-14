#include "vote_widget.h"
#include "ui_vote_widget.h"

VoteWidget::VoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoteWidget)
{
    ui->setupUi(this);
    on_plusButton_clicked();
    on_plusButton_clicked();
}

VoteWidget::~VoteWidget()
{
    delete ui;
}

void VoteWidget::updateResults(VoteResults results)
{

}

void VoteWidget::on_plusButton_clicked()
{
    QString name = QString("A%1:").arg(ui->customLayout->rowCount()+1);
    answers.append(new QLineEdit(this));
    ui->customLayout->addRow(name, answers.last());
}

void VoteWidget::on_pushButton_toggled(bool checked)
{
    if (!checked)
    {
        emit voteStop();
        ui->pushButton->setText(tr("Start"));
    }
    else
    {
        if (ui->simpleRB->isChecked())
            // Simple case
            emit voteNew(VotingInvite(ui->questionEdit->text()));
        else
        {
            QStringList answersText;
            foreach (QLineEdit *l, answers) {
                answersText.append(l->text());
            }
            // Custom case
            emit voteNew(VotingInvite(ui->questionEdit->text(),
                                      VotingInvite::Custom,
                                      answersText));
        }
        ui->pushButton->setText(tr("Stop"));
    }
}
