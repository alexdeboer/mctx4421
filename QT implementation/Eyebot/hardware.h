#ifndef HARDWARE_H
#define HARDWARE_H

#include <QString>

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

struct motor {
    QString name;
    int freq;
    int speed;
    int direction;
    QString table;
    int RegAddr;
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

struct servos {
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
    int find_motors();
    int find_encoders();
    int find_psds();
    int find_irtvs();
    int find_servos();
    int find_adcs();
    int find_camera();


};

#endif // HARDWARE_H
