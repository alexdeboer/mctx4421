#include "software.h"

Software::Software()
{
}

QString Software::run_Test() {

    QString ret;
    QProcess process;
    /*QStringList arguments;
    arguments << "-c";
    arguments << "/home/pi/EyeBot_BB/demos/helloWorld/helloWorld";
    process.execute("bash", arguments);
    process.waitForFinished(-1); // will wait forever until finished
*/

    process.execute("/Users/Markcuz/Qt5.2.1/5.2.1/Src/qtmultimedia/examples/multimediawidgets/build-player-Desktop_Qt_5_2_1_clang_64bit-Debug/player");
    process.startDetached("/Users/Markcuz/Qt5.2.1/5.2.1/Src/qtmultimedia/examples/multimediawidgets/build-player-Desktop_Qt_5_2_1_clang_64bit-Debug/player");
    ret = process.readAllStandardError();

    process.waitForFinished();
    return ret;
}
