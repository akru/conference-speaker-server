#ifndef VOTE_RESULTS_WIDGET_H
#define VOTE_RESULTS_WIDGET_H

#include <QWidget>
#include <voting.h>

namespace Ui {
class VoteResultsWidget;
}

class VoteResultsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VoteResultsWidget(QWidget *parent = 0);
    ~VoteResultsWidget();

public slots:
    void voteUpdateResults(VoteResults results);

private:
    Ui::VoteResultsWidget *ui;
};

#endif // VOTE_RESULTS_WIDGET_H
