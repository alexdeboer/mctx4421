#ifndef SOFTWARE_H
#define SOFTWARE_H

#include <QProcess>
#include <QList>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFileInfo>

class Software
{
public:
    Software();
    void init();

    //QList progList();
   // QString run_Test();
    QFileInfoList showList();

private:
    QFileInfoList list;

public slots:
    void run(int current);
};

#endif // SOFTWARE_H
