#include "software.h"

Software::Software()
{
}

void Software::init() {
    QString path = "/Users/Markcuz/Desktop/";
    QDir directory = QDir(path);
    QFileInfoList tempList =  directory.entryInfoList();

    list.clear();

    for(int i =0; i<tempList.size(); i++) {
        if(tempList.at(i).isFile()) {
            list.append(tempList.at(i));
        }
    }
}

QFileInfoList Software::showList() {
    return list;
}

void Software::run(int current) {
    qDebug() << list.at(current).absoluteFilePath();
}
