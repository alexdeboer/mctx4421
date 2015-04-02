#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <QProcess>

class Commands
{
public:
    Commands();
    void shutdown();
    void restart();
    void exit();
};

#endif // COMMANDS_H
