#include "info.h"

Info::Info()
{

}

QString Info::processor() {
    QString processor = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << "cat /proc/cpuinfo |grep Processor|cut -d ':' -f2";
    process.start("bash", arguments);
    process.waitForFinished(-1);
    processor = process.readAllStandardOutput();
#endif
    processor.remove(0,1);
    return processor;
}

QString Info::speed() {
    QString speed = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << "cat /proc/cpuinfo |grep BogoMIPS|cut -d ':' -f2";
    process.start("bash", arguments);
    process.waitForFinished(-1);
    speed = process.readAllStandardOutput();
    speed.append(" MHz");
#endif
    speed.remove(0,1);
    return speed;
}

QString Info::arch() {
    QString arch = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << "cat /proc/cpuinfo |grep Processor|cut -d' ' -f2|cut -d '-' -f1";
    process.start("bash", arguments);
    process.waitForFinished(-1);
    arch = process.readAllStandardOutput();
#endif
    return arch;
}

QString Info::bogo() {
    QString bogo = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << "cat /proc/cpuinfo |grep BogoMIPS|cut -d ':' -f2";
    process.start("bash", arguments);
    process.waitForFinished(-1);
    bogo = process.readAllStandardOutput();
#endif
    bogo.remove(0,1);
    return bogo;
}

QString Info::processes() {
    QString processes = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("ps aux | wc -l");
    process.start("bash", arguments);
    process.waitForFinished(-1);
    processes = process.readAllStandardOutput();
#endif
    return processes;
}

QString Info::totalRam() {
    QString totalRam = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("cat /proc/meminfo |grep MemTotal|cut -d':' -f2");
    process.start("bash", arguments);
    process.waitForFinished(-1);
    totalRam = process.readAllStandardOutput();

    int i = 0;
    while(totalRam.at(i)==' ') {
        i++;
    }
    totalRam.remove(0,i);
#endif
    return totalRam;
}

QString Info::freeRam() {
    QString freeRam;
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("cat /proc/meminfo |grep MemFree|cut -d':' -f2");
    process.start("bash", arguments);
    process.waitForFinished(-1);
    freeRam = process.readAllStandardOutput();

    int i = 0;
    while(freeRam.at(i)==' ') {
        i++;
    }
    freeRam.remove(0,i);
#endif
    return freeRam;
}

QString Info::upTime() {
    QString uptime = "";
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("cat /proc/uptime|cut -d '.' -f1");
    process.start("bash", arguments);
    process.waitForFinished(-1);
    uptime = process.readAllStandardOutput();

    uptime.remove(uptime.length()-1,uptime.length());
    uptime.append("s");
#endif
    return uptime;
}
