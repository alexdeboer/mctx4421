#ifndef INFO_H
#define INFO_H

#include <QString>
#include <QProcess>
#include <QDebug>

class System
{
public:
    System();
    QString processor();
    QString speed();
    QString arch();
    QString bogo();
    QString processes();
    QString totalRam();
    QString freeRam();
    QString upTime();
};

#endif // INFO_H
