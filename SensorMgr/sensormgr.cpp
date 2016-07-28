#include "sensormgr.h"
#include <QtCore/qmath.h>

SensorMgr::SensorMgr()
{

}

void SensorMgr::stop()
{
    m_mutex.lock();
    m_stop=true;
    m_mutex.unlock();
}


void SensorMgr::run()
{
    static int recordid = 0;

#ifndef __arm__
    /** simulator data */
    for (int i= 0; i < 4; i++)
    {
        Sensor s;
        s.decription = "decription " + QString::number(i);
        s.type = "Ultrasonic";
        s.id = i;
        sharedMemMgr.addSensor(s);
    }
#endif

    while (isRunning())
    {
        m_mutex.lock();
        if (m_stop) break;
        m_mutex.unlock();

#ifndef __arm__
        /** simulator data */
        Record r;
        r.recordID = recordid++;
        r.value[0] = qSin(r.recordID / 1000.0);
        r.value[1] = qSin(r.recordID / 100.0);
        r.value[2] = qSin(r.recordID / 10.0);
        r.value[3] = qSin(r.recordID / 1.0);

        sharedMemMgr.addRecord(r);
        emit sng_recordAdded();

        msleep(30);
#else
#endif
    }

}

SensorMgr sensorMgr;
