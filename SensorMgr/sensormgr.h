#ifndef SENSORMGR_H
#define SENSORMGR_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "SharedMemMgr/sharedmemmgr.h"

class SensorMgr: public QThread
{
    Q_OBJECT
public:
    SensorMgr();
    void stop();

    void run();

private:
    QMutex m_mutex;
    bool m_stop;
signals:
    void sng_recordAdded();

};

extern SensorMgr sensorMgr;

#endif // SENSORMGR_H
