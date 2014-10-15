#include "vote_widget.h"
#include "ui_vote_widget.h"

static const char *resultQuestionText = "Question: %1";
static const char *resultLabelText    = "%1 / %2%";

VoteWidget::VoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoteWidget),
    resultWidget(new QWidget)
{
    ui->setupUi(this);

    resultWidget->setLayout(new QGridLayout(resultWidget));
    resultWidget->setWindowTitle(tr("Results"));
    resultWidget->setWindowFlags(Qt::WindowTitleHint);

    on_plusButton_clicked();
    on_plusButton_clicked();
    enableCustom(false);
    connect(ui->customRB, SIGNAL(toggled(bool)), SLOT(enableCustom(bool)));
}

VoteWidget::~VoteWidget()
{
    delete ui;
    delete resultWidget;
}

void VoteWidget::updateResults(VoteResults results)
{
    // Set question
    ui->resultQuestion->setText(tr(resultQuestionText).arg(results.invite.question));
    // Clear fields
    foreach (QLabel *l, resultAnswers) {
        delete l;
    }
    resultAnswers.clear();
    while (ui->formLayout_2->itemAt(0) != 0)
    {
        QLayoutItem *i = ui->formLayout_2->takeAt(0);
        delete i->widget();
        delete i;
    }
    // Calc count of members
    int sumAnswers;
    foreach (int a, results.values) {
        sumAnswers += a;
    }
    // Small hack for divByZero exception
    if (!sumAnswers) sumAnswers = 1;
    // Draw fields
    if (results.invite.mode == VotingInvite::Simple)
    {
        resultAnswers.append(
                    new QLabel(tr(resultLabelText)
                               .arg(results.values[0])
                               .arg(results.values[0] * 100.0 / sumAnswers), this));
        ui->formLayout_2->addRow(tr("Yes"), resultAnswers.last());
        resultAnswers.append(
                    new QLabel(tr(resultLabelText)
                               .arg(results.values[1])
                               .arg(results.values[1] * 100.0 / sumAnswers), this));
        ui->formLayout_2->addRow(tr("No"), resultAnswers.last());
    }
    else
    {
        for (short i = 0; i < results.values.length()
                       && i < results.invite.answers.length(); ++i)
        {
            resultAnswers.append(
                        new QLabel(tr(resultLabelText)
                                   .arg(results.values[i])
                                   .arg(results.values[i] * 100.0 / sumAnswers), this));
            ui->formLayout_2->addRow(results.invite.answers[i], resultAnswers.last());
        }
    }
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
                if (!l->text().isEmpty())
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

void VoteWidget::enableCustom(bool enable)
{
    foreach (QLineEdit *l, answers) {
        l->setEnabled(enable);
    }
    ui->plusButton->setEnabled(enable);
}

void VoteWidget::on_pushButton_2_toggled(bool checked)
{
    if (checked)
    {
        ui->groupBox_2->setParent(resultWidget);
        resultWidget->layout()->addWidget(ui->groupBox_2);
        resultWidget->show();
    }
    else
    {
        ui->groupBox_2->setParent(this);
        ui->gridLayout->addWidget(ui->groupBox_2, 1, 0, 1, 1);
        resultWidget->hide();
    }
}
