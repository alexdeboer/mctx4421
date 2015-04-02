#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include "home.h"
#include "system.h"
#include "commands.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings settings;
    Home home;
    System system;
    Commands commands;

private slots:
    //initialising the settings
    void init_home();

    void init_system();
    void init_network();
    void init_settings();

    void init_software();

    void init_hardware();

    //initialising the UI
    void init_ui();

    //showing the info on the screen
    void show_home();

    void show_system();
    void show_network();
    void show_settings();

    void show_hardware();

    void show_software();

    void show_commands();

    //update functions
    void update_system();
};

#endif // MAINWINDOW_H
