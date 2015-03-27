#include "mainwindow.h"
#include "ui_mainwindow.h"

#define VERSION "27/03/15"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_home();

    //initialise the main screen
    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*---------------------Initialisation------------------*/

void MainWindow::init_home() {
    settings.setValue("home/ip", home.ip());
    settings.setValue("home/hostname", home.hostname());
    settings.setValue("home/macaddr", home.macaddr());
    settings.setValue("home/ssid",home.ssid());
    settings.setValue("home/wifiip", home.wifiip());
    settings.setValue("home/robios", VERSION);
    settings.setValue("home/battery", home.batt());
}

void MainWindow::init_info() {
    settings.setValue("info/processor", home.batt());
}

void MainWindow::init_hardware() {

}

void MainWindow::init_software() {

}

/*-----------------Initialises the UI-----------------*/

void MainWindow::init_ui() {
    //displays the home page
    show_home();
}


/*----------------Displays the widgets----------------*/

void MainWindow::show_home() {
    //hiding all the widgets
    ui->home->show(); ui->home->setEnabled(true); ui->menu_home->show(); ui->menu_home->setEnabled(true);
    ui->info->hide();ui->info->setEnabled(false); ui->menu_info->hide(); ui->menu_info->setEnabled(false);
    ui->tempBlack->hide(); 

    //sets the labels
    ui->label_ip->setText(settings.value("home/ip").toString());
    ui->label_hostname->setText(settings.value("home/hostname").toString());
    ui->label_macaddr->setText(settings.value("home/macaddr").toString());
    ui->label_ssid->setText(settings.value("home/ssid").toString());
    ui->label_wifiip->setText(settings.value("home/wifiip").toString());
    ui->label_robios->setText(settings.value("home/robios").toString());
    ui->label_batt->setText(settings.value("home/battery").toString());

    //connects the buttons
    connect(ui->home_info, SIGNAL(clicked()), this, SLOT(show_info()));
    connect(ui->home_hardware, SIGNAL(clicked()), this, SLOT(show_hardware()));
    connect(ui->home_software, SIGNAL(clicked()), this, SLOT(show_software()));
    connect(ui->home_demos, SIGNAL(clicked()), this, SLOT(show_demos()));
}

void MainWindow::show_info() {
    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);
    ui->info->show(); ui->info->setEnabled(true);ui->menu_info->show(); ui->menu_info->setEnabled(true);
    ui->tempBlack->hide();


    connect(ui->info_back, SIGNAL(clicked()), this, SLOT(show_home()));
}

void MainWindow::show_hardware() {

}

void MainWindow::show_software() {

}

void MainWindow::show_demos() {

}

