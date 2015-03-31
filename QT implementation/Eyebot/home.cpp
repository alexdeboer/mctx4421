#include "home.h"

Home::Home()
{

}

QString Home::batt() {
    //had a read through the old code, doesnt do anything =/
    return "5V          100%";
}


QString Home::ip() {
    QString ip = " ";
#ifndef TEST
    QProcess findip;
    QStringList arguments;
    arguments << "-c";
    arguments << "ifconfig eth0|grep inet|cut -d ':' -f2|cut -d ' ' -f1";
    findip.start("bash", arguments);
    findip.waitForFinished(-1); // will wait forever until finished
    ip = findip.readAllStandardOutput();
#endif
    return ip;
}

QString Home::hostname() {
    QString hostname = " ";
#ifndef TEST
    char host[70];
    host[69] = '\0';
    gethostname(host,70);

    hostname = QString::fromUtf8(host);
#endif
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
    QStringList arguments;
    arguments << "-c";
    arguments << "cat /proc/cpuinfo|grep Serial|cut -d' ' -f2";
    process.start("bash", arguments);
    process.waitForFinished(-1);
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
    /*
    process.start("sudo service hostapd restart");
    process.waitForFinished(-1);
    process.start("sudo service isc-dhcp-server restart");
    process.waitForFinished(-1);

    */
#endif
    return ssid;
}

QString Home::wifiip() {
    return "10.1.1.1";
}

