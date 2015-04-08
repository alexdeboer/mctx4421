#ifndef HARDWARE_H
#define HARDWARE_H

#include <QString>
#include <QList>
#include <QFile>

#define defaultFilePath "/home/pi/EyeBot_Pi/hardware.txt"

struct motor {
    QString name;
    int freq;
    int speed;
    int direction;
    QString table;
    int RegAddr;
};

struct encoder {
    QString name;
    int clicks;
    motor motorNumber;
    int decoder;
    int speed;
    int direction;
};

struct psd {
    QString name;
    int rawValue;
    int tableValue;
    QString table;
    int RegAddr;
};

struct irtv {
    QString name;
    int length;
    int toggleMask;
    int invertMask;
    QString mode;
    int bufferSize;
    int delay;
};

struct servo {
    QString name;
    int freq;
    int minPWM;
    int maxPWM;
    int currrentValue;
    int RegAddr;
};

struct camera {
    QString name;
    int videoLength;
    int resolution;
};


class Hardware
{
public:
    Hardware();
    QList<motor> motors;
    QList<encoder> encoders;
    QList<psd> psds;
    QList<irtv> irtvs;
    QList<servo> servos;
   // QList<adc> adcs;
    QList<camera> cameras;

    void init();

private:
    void add_motors();
    void add_encoders();
    void add_psds();
    void add_irtvs();
    void add_servos();
    void add_adcs();
    void add_camera();

};

#endif // HARDWARE_H
