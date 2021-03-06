#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QDebug>

#include "home.h"
#include "system.h"
#include "commands.h"
#include "software.h"

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
    Software software;

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

    void show_info();
    void show_system();
    void show_network();
    void show_settings();

    void show_hardware();

    void show_software();

    void show_commands();

    //exectution functions
    void run_software();

    //update functions
    void refresh_software();
    void update_system();
};

#endif // MAINWINDOW_H
