#include "sensormgr.h"
#include <QtCore/qmath.h>

SensorMgr::SensorMgr():
    m_stop(false),
    deviceMonitor(new DeviceMonitor(this))
{
    connect(deviceMonitor, SIGNAL(devideAdded(int,QString)),
            this, SLOT(slot_SensorAdded(int,QString)));
    connect(deviceMonitor, SIGNAL(devideRemoved(int,QString)),
            this, SLOT(slot_SensorRemoved(int,QString)));
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

void SensorMgr::slot_SensorAdded(int usbPort, QString name)
{
    qDebug() << Q_FUNC_INFO << " usbPort = " << usbPort
             << " name = " << name;
    Sensor s;
    s.decription = "decription";
    s.id = usbPort;
    s.type = "ultrasonic";
    s.devName = "/dev/" + name;

    sharedMemMgr.addSensor(s);
    listOfReaderThread.append(new SensorReader(this, s.id, s.devName));
}

void SensorMgr::slot_SensorRemoved(int usbPort, QString name)
{
    qDebug() << Q_FUNC_INFO << " usbPort = " << usbPort
             << " name = " << name;
    sharedMemMgr.removeSensor(usbPort);
    for (int i =0; i < listOfReaderThread.length(); i++)
    {
        SensorReader * t = listOfReaderThread.at(i);
        if (t->getId() == usbPort)
        {
            t->terminate();
        }
        listOfReaderThread.removeAt(i);
        connect(t, SIGNAL(sgn_Finished(SensorReader*)),
                this, SLOT(slot_ReaderFinished(SensorReader*)));
    }

}

void SensorMgr::slot_ReaderFinished(SensorReader *t)
{
    qDebug() << "Delete " << t;
    delete t;
}
