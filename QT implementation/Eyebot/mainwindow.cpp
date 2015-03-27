#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialise the main screen
    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_ui() {
    ui->home->show(); ui->home->setEnabled(true);ui->menu_home->show(); ui->menu_home->setEnabled(true);
    ui->info->hide();
    ui->tempBlack->hide();

    connect(ui->pushButton_info, SIGNAL(clicked()), this, SLOT(show_info()));
}

void MainWindow::show_info() {
    ui->home->hide(); ui->home->setEnabled(false); ui->menu_home->hide(); ui->menu_home->setEnabled(false);
    ui->info->show(); ui->info->setEnabled(true);ui->menu_info->show(); ui->menu_info->setEnabled(true);
    ui->tempBlack->hide();

    connect(ui->pushButton_home, SIGNAL(clicked()), this, SLOT(init_ui()));
}





