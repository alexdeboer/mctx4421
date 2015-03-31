#ifndef HOME_H
#define HOME_H

#include <QString>
#include <QProcess>
#include <unistd.h>
#include <QDebug>

class Home
{
public:
    Home();

    QString batt();
    QString ip();
    QString hostname();
    QString macaddr();
    QString ssid();
    QString wifiip();
};

#endif // HOME_H
