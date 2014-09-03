#include "vote_widget.h"
#include "ui_vote_widget.h"

VoteWidget::VoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoteWidget),
    count(0)
{
    ui->setupUi(this);
    connect(ui->yesBar, SIGNAL(valueChanged(int)), SLOT(updateYesL(int)));
    connect(ui->noBar, SIGNAL(valueChanged(int)), SLOT(updateNoL(int)));
}

VoteWidget::~VoteWidget()
{
    delete ui;
}

void VoteWidget::appendClient()
{
    setCount(count + 1);
}

void VoteWidget::dropClient()
{
    setCount(count - 1);
}

void VoteWidget::updateYesL(int count)
{
    ui->yesL->setText(QString::number(count) + "%");
}

void VoteWidget::updateNoL(int count)
{
    ui->noL->setText(QString::number(count) + "%");
}

void VoteWidget::vote(QString address, bool type)
{
    if (voters.contains(address))
        // Emit denied signal when double vote
        emit voteDenied(address);
    else
    {
        // Append voter to list
        voters.append(address);
        // Increment counter value
        if (type)
            appendYes();
        else
            appendNo();
        // Emit accepted signal
        emit voteAccepted(address);
    }
}

void VoteWidget::setCount(int count)
{
    float scale = this->count;
    scale /= count;                             // Calculate scale param

    int yesVal = ui->yesBar->value() * scale ;  // Change yes base
    int noVal = ui->noBar->value() * scale;     // Change no base

    ui->yesBar->setValue(yesVal);
    ui->noBar->setValue(noVal);

    this->count = count;                        // Store new count
}

void VoteWidget::appendYes()
{
    if (voters.count() < count)
    {
        int oneByPerc = 1.0 / count * 100;
        ui->yesBar->setValue(ui->yesBar->value() + oneByPerc);
    }
}

void VoteWidget::appendNo()
{
    if (voters.count() < count)
    {
        int oneByPerc = 1.0 / count * 100;
        ui->noBar->setValue(ui->noBar->value() + oneByPerc);
    }
}

void VoteWidget::on_voteButton_clicked()
{
    voters.clear();
    ui->yesBar->setValue(0);
    ui->noBar->setValue(0);
}
