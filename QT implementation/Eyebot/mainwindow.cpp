#include "mainwindow.h"
#include "ui_mainwindow.h"

#define VERSION "27/03/15"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_home();
    init_system();
    init_network();
    init_software();

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

void MainWindow::init_system() {

    settings.setValue("system/processor", system.processor());
    settings.setValue("system/speed", system.speed());
    settings.setValue("system/arch", system.arch());
    settings.setValue("system/bogo", system.bogo());
    settings.setValue("system/totalRam", system.totalRam());
    settings.setValue("system/freeRam", system.freeRam());
    settings.setValue("system/uptime", system.upTime());
    settings.setValue("system/processes", system.processes());
    settings.setValue("system/battery", settings.value("home/battery"));

}

void MainWindow::init_network() {
    settings.setValue("network/ip", settings.value("home/ip"));
    settings.setValue("network/hostname", settings.value("home/hostname"));
    settings.setValue("network/macaddr", settings.value("home/macaddr"));
    settings.setValue("network/ssid",settings.value("home/ssid"));
    settings.setValue("network/wifiip", settings.value("home/wifiip"));
}

void MainWindow::init_settings() {

}

void MainWindow::init_hardware() {

    //connect(ui->hardware_select, SIGNAL(clicked()), this, SLOT(show_system()));
    //connect(ui->hardware_reload, SIGNAL(clicked()), this, SLOT(show_home()));
    //connect(ui->hardware_io, SIGNAL(clicked()), this, SLOT(show_network()));
    connect(ui->hardware_back, SIGNAL(clicked()), this, SLOT(show_home()));
}

void MainWindow::init_software() {
    software.init();
    connect(ui->software_run, SIGNAL(clicked()), this, SLOT(run_software()));
    connect(ui->software_refresh, SIGNAL(clicked()), this, SLOT(refresh_software()));
    connect(ui->software_back, SIGNAL(clicked()), this, SLOT(show_home()));
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

    ui->info->hide(); ui->info->setEnabled(false); ui->menu_info->hide(); ui->menu_info->setEnabled(false);

    ui->hardware->hide(); ui->hardware->setEnabled(false); ui->menu_hardware->hide(); ui->menu_hardware->setEnabled(false);
    ui->software->hide(); ui->software->setEnabled(false); ui->menu_software->hide(); ui->menu_software->setEnabled(false);
    ui->commands->hide(); ui->commands->setEnabled(false); ui->menu_commands->hide(); ui->menu_commands->setEnabled(false);

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
    connect(ui->home_commands, SIGNAL(clicked()), this, SLOT(show_commands()));
}

void MainWindow::show_info() {
    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);

    ui->info->show(); ui->info->setEnabled(true); ui->menu_info->show(); ui->menu_info->setEnabled(true);

    ui->system->show();

    ui->hardware->hide(); ui->hardware->setEnabled(false); ui->menu_hardware->hide(); ui->menu_hardware->setEnabled(false);
    ui->software->hide(); ui->software->setEnabled(false); ui->menu_software->hide(); ui->menu_software->setEnabled(false);
    ui->commands->hide(); ui->commands->setEnabled(false); ui->menu_commands->hide(); ui->menu_commands->setEnabled(false);

    connect(ui->info_network, SIGNAL(clicked()), this, SLOT(show_network()));
    connect(ui->info_back, SIGNAL(clicked()), this, SLOT(show_home()));
    connect(ui->info_settings, SIGNAL(clicked()), this, SLOT(show_settings()));
    connect(ui->info_system, SIGNAL(clicked()), this, SLOT(show_system()));
}

void MainWindow::show_system() {
    ui->system->show(); ui->system->setEnabled(true);
    ui->network->hide(); ui->network->setEnabled(false);
    ui->settings->hide();ui->settings->setEnabled(false);

    //set labels

    ui->label_arch->setText(settings.value("system/arch").toString());
    ui->label_processor->setText(settings.value("system/processor").toString());
    ui->label_speed->setText(settings.value("system/speed").toString());
    ui->label_bogo->setText(settings.value("system/bogo").toString());
    ui->label_processes->setText(settings.value("system/processes").toString());
    ui->label_totalRam->setText(settings.value("system/totalRam").toString());
    ui->label_freeRam->setText(settings.value("system/freeRam").toString());
    ui->label_uptime->setText(settings.value("system/uptime").toString());
    ui->label_batt2->setText(settings.value("system/battery").toString());

/*  The timer method, slows down the oprogram too much (due to calling external processes)
    QTimer *updateTimer;
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update_system()));
    updateTimer->start(2000);

    */
}

void MainWindow::show_network() {

    ui->system->hide(); ui->system->setEnabled(false);
    ui->network->show(); ui->network->setEnabled(true);
    ui->settings->hide(); ui->settings->setEnabled(false);

    //set labels

    ui->label_ip2->setText(settings.value("home/ip").toString());
    ui->label_hostname2->setText(settings.value("home/hostname").toString());
    ui->label_macaddr2->setText(settings.value("home/macaddr").toString());
    ui->label_ssid2->setText(settings.value("home/ssid").toString());
    ui->label_wifiip2->setText(settings.value("home/wifiip").toString());
}

void MainWindow::show_settings() {

    ui->system->hide();ui->system->setEnabled(false);
    ui->network->hide(); ui->network->setEnabled(false);
    ui->settings->show();ui->settings->setEnabled(true);
}


void MainWindow::show_hardware() {

    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);

    ui->info->hide(); ui->info->setEnabled(false); ui->menu_info->hide(); ui->menu_info->setEnabled(false);

    ui->hardware->show(); ui->hardware->setEnabled(true); ui->menu_hardware->show(); ui->menu_hardware->setEnabled(true);
    ui->software->hide(); ui->software->setEnabled(false); ui->menu_software->hide(); ui->menu_software->setEnabled(false);
    ui->commands->hide(); ui->commands->setEnabled(false); ui->menu_commands->hide(); ui->menu_commands->setEnabled(false);
}

void MainWindow::show_software() {
    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);

    ui->info->hide(); ui->info->setEnabled(false); ui->menu_info->hide(); ui->menu_info->setEnabled(false);

    ui->hardware->hide(); ui->hardware->setEnabled(false); ui->menu_hardware->hide(); ui->menu_hardware->setEnabled(false);
    ui->software->show(); ui->software->setEnabled(true); ui->menu_software->show(); ui->menu_software->setEnabled(true);
    ui->commands->hide(); ui->commands->setEnabled(false); ui->menu_commands->hide(); ui->menu_commands->setEnabled(false);

    //adding elements to the table
    int listSize = software.showList().size();

    for(int i = 0; i<listSize; i++) {
        ui->table_software->insertRow(i);
        QTableWidgetItem *newItem = new QTableWidgetItem(software.showList().at(i).fileName());
        ui->table_software->setItem(i,0,newItem);
    }

    if(listSize!=0) {
        ui->table_software->setCurrentCell(0,0);
    }
}

void MainWindow::refresh_software() {
    software.init();

    int rows = ui->table_software->rowCount();

    for(int i = 0; i<rows; i++) {
        ui->table_software->removeRow(0);
    }

    show_software();
}

void MainWindow::run_software() {
    if(software.showList().size()!=0) {
        int current = ui->table_software->currentRow();
        software.run(current);
    }
}

void MainWindow::show_commands() {

    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);

    ui->hardware->hide(); ui->hardware->setEnabled(false); ui->menu_hardware->hide(); ui->menu_hardware->setEnabled(false);
    ui->software->hide(); ui->software->setEnabled(false); ui->menu_software->hide(); ui->menu_software->setEnabled(false);
    ui->commands->show(); ui->commands->setEnabled(true); ui->menu_commands->show(); ui->menu_commands->setEnabled(true);

    /*connect(ui->commands_shutdown, SIGNAL(clicked()), &commands, SLOT(shutdown()));
    connect(ui->commands_restart, SIGNAL(clicked()), this, SLOT(commands.restart()));
    connect(ui->commands_exit, SIGNAL(clicked()), this, SLOT(commands.exit()));
    */connect(ui->commands_back, SIGNAL(clicked()), this, SLOT(show_home()));

}

//Update functions
void MainWindow::update_system() {

    settings.setValue("system/freeRam", system.freeRam());
    settings.setValue("system/uptime", system.upTime());
    settings.setValue("system/processes", system.processes());

    ui->label_processes->setText(settings.value("system/processes").toString());
    ui->label_uptime->setText(settings.value("system/uptime").toString());
    ui->label_freeRam->setText(settings.value("system/freeRam").toString());

}

