#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void appendUser(UserInformation info);
    void dropUser(UserInformation info);

private:
    Ui::MainWindow *ui;
    Server *s;
};

#endif // MAIN_WINDOW_H
