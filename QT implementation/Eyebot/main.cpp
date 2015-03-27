#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("UWA Robotics");
    QCoreApplication::setApplicationName("Eyebot RPi");

    MainWindow w;
    w.show();

    return a.exec();
}
