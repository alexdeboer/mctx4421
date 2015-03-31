#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include "home.h"
#include "info.h"

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
    Info info;

private slots:
    //initialising the settings
    void init_home();
    void init_info();
    void init_software();
    void init_hardware();

    //initialising the UI
    void init_ui();

    //showing the info on the screen
    void show_info();
    void show_home();
    void show_hardware();
    void show_software();
    void show_demos();

    //update functions
    void update_Info();
};

#endif // MAINWINDOW_H
