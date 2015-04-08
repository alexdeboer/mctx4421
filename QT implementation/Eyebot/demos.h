#ifndef DEMOS_H
#define DEMOS_H

#include <QList>
#include <QString>

struct demoElement {
    QString name;
    QString address;
};


class Demos
{
public:
    Demos();
    //QList demos_list();
    void init();
};

#endif // DEMOS_H
