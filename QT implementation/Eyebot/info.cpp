#include "info.h"

Info::Info()
{

}

QString Info::processor() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep Processor|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif
}

QString Info::speed() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep |cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif
}

QString Info::arch() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep vendor_id|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif
}

QString Info::bogo() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep BogoMIPS|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif
}

QString Info::processes() {
#ifndef TEST
    QProcess process;
    process.start("ps aux | wc -l");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif
}

QString Info::totalRam() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/meminfo |grep MemTotal|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif

}

QString Info::freeRam() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep MemFree|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif

}

QString Info::upTime() {
#ifndef TEST
    QProcess process;
    process.start("cat /proc/uptime");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();
    return stdout;
#endif

}
