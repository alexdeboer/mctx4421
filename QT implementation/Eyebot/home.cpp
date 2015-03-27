#include "home.h"

Home::Home()
{

}

QString Home::batt() {
    //had a read through the old code, doesnt do anything =/
    return "5V      100%";
}


QString Home::ip() {
    QProcess findip;
    findip.start("ifconfig eth0 | sed -rn 's/.*r:([^ ]+) .*/\\1/p' | awk '{printf \"%s \",$0} END {print \"\"}'");
    findip.waitForFinished(-1); // will wait forever until finished
    QString stdout = findip.readAllStandardOutput();
    return stdout;
}

QString Home::hostname() {
    char hostname[70];
    hostname[69] = '\0';
    gethostname(hostname,70);
    return hostname;
}

QString Home::macaddr() {
    QString macaddr = "";
#ifndef TEST
    QProcess process;
    process.start("cat /sys/class/net/eth0/address");
    process.waitForFinished();
    macaddr = process.readAllStandardOutput();
#endif
    return macaddr;
}

QString Home::ssid() {
    QString ssid = "Pi_";
#ifndef TEST
    QProcess process;
    process.start("cat /proc/cpuinfo |grep Serial|cut -d' ' -f2");
    process.waitForFinished();
    QString stdout = process.readAllStandardOutput();

    int i = 0;

    while(stdout.at(i) == '0') {
        i++;
    }

    for(i;i<stdout.length();i++) {
        ssid.append(stdout.at(i));
    }

    //changing the ssid
    QString cmd = "sed -i '3s/.* /ssid=";
    cmd.append(ssid);
    cmd.append("/' /etc/hostapd/hostapd.conf");
    process.start(cmd);
    process.waitForFinished();

    //restarting the services
    process.start("sudo service hostapd restart");
    process.start("sudo service isc-dhcp-server restart");
#endif
    return ssid;
}

QString Home::wifiip() {
    return "10.1.1.1";
}

