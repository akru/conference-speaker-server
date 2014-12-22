#ifndef SETTINGS_H
#define SETTINGS_H

#include <server_information.h>
#include <QObject>

namespace Ui {
class MainWindow;
class SoundExpertMode;
class SoundUserMode;
}

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings(Ui::MainWindow *ui,
             Ui::SoundExpertMode *ex,
             Ui::SoundUserMode *us,
             QObject *parent = 0);

    /* Settings rows */
    // Server information loaded
    ServerInformation info();
    /* Settings rows end */

signals:
    void settingsSaved();

public slots:
    // Setting save slot (dumps settings rows to disk)
    void save();
    void saveAsDialog();
    void saveAs(QString fname);

    void load();
    void loadAsDialog();
    void loadDefault();
    void loadAs(QString fname);

private slots:
    void usNoiseControl();

private:
    QString             filename;
    Ui::MainWindow      *ui;
    Ui::SoundExpertMode *ex;
    Ui::SoundUserMode   *us;
};

#endif // SETTINGS_H
