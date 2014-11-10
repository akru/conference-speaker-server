#ifndef SETTINGS_H
#define SETTINGS_H

#include <server_information.h>
#include <QObject>

namespace Ui {
class MainWindow;
}

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings(Ui::MainWindow *ui, QObject *parent = 0);

    /* Settings rows */
    // Server information loaded
    ServerInformation info();
    /* Settings rows end */

signals:
    void settingsSaved();

public slots:
    // Setting save slot (dumps settings rows to disk)
    void save();

private:
    QString filename;
    Ui::MainWindow *ui;
};

#endif // SETTINGS_H
