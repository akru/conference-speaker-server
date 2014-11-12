#include "vote_results_widget.h"
#include "ui_vote_results_widget.h"

VoteResultsWidget::VoteResultsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoteResultsWidget)
{
    ui->setupUi(this);
}

VoteResultsWidget::~VoteResultsWidget()
{
    delete ui;
}

void VoteResultsWidget::voteUpdateResults(VoteResults results)
{
    static const char *resultLabelText    = "%1 / %2%";
    // Set question
    ui->resultQuestion->setText(results.invite.question);
    // Clear fields
    while (ui->resultsLayout->itemAt(0) != 0)
    {
        QLayoutItem *i = ui->resultsLayout->takeAt(0);
        delete i->widget();
        delete i;
    }
    // Calc count of members
    int sumAnswers = 0;
    foreach (int a, results.values) {
        sumAnswers += a;
    }
    // Small hack for divByZero exception
    if (!sumAnswers) sumAnswers = 1;
    // Draw fields
    QLabel *answer;
    if (results.invite.mode == VotingInvite::Simple)
    {
        answer = new QLabel(tr(resultLabelText)
                            .arg(results.values[0])
                            .arg(results.values[0] * 100.0 / sumAnswers), this);
        ui->resultsLayout->addRow(tr("Yes"), answer);
        answer = new QLabel(tr(resultLabelText)
                            .arg(results.values[1])
                            .arg(results.values[1] * 100.0 / sumAnswers), this);
        ui->resultsLayout->addRow(tr("No"), answer);
    }
    else
    {
        for (short i = 0; i < results.values.length() &&
                          i < results.invite.answers.length(); ++i)
        {
            answer = new QLabel(tr(resultLabelText)
                                .arg(results.values[i])
                                .arg(results.values[i] * 100.0 / sumAnswers), this);
            ui->resultsLayout->addRow(results.invite.answers[i], answer);
        }
    }
    ui->votingDateEdit->setDateTime(QDateTime::currentDateTime());
}
