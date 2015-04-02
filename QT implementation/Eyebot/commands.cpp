#include "commands.h"

Commands::Commands()
{
}

void Commands::shutdown() {
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("sudo halt");
    process.start("bash", arguments);
#endif
}

void Commands::restart() {
#ifndef TEST
    QProcess process;
    QStringList arguments;
    arguments << "-c";
    arguments << ("sudo restart");
    process.start("bash", arguments);
#endif
}

void Commands::exit() {
    /*
    QMessageBox msgBox;
        msgBox.setText("Are you sure you want to quit?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int rec = msgBox.exec();
        switch(rec){
        case QMessageBox::Yes:
            QApplication::quit();
            break;
        case QMessageBox::No:
            break;
        default:
            break;
        }
        */
}


