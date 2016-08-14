#include "sensormgr.h"
#include <QtCore/qmath.h>

SensorMgr::SensorMgr():
    m_stop(false),
    deviceMonitor(new DeviceMonitor(this)),
    needToDeleteThread(0),
    currentRecordID(0)
{
    connect(deviceMonitor, SIGNAL(devideAdded(int,QString)),
            this, SLOT(slot_SensorAdded(int,QString)));
    connect(deviceMonitor, SIGNAL(devideRemoved(int,QString)),
            this, SLOT(slot_SensorRemoved(int,QString)));

    deviceMonitor->getListPrePlugPorts();
}

void SensorMgr::stop()
{
    m_mutex.lock();
    m_stop=true;
    m_mutex.unlock();
}

void SensorMgr::run()
{

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
    Sensor_t * sensor = new Sensor_t;
    sensor->reader = new SensorReader(this, s.id, s.devName);
    connect(sensor->reader, SIGNAL(sgn_NewData(int,float)),
            this, SLOT(slot_NewData(int,float)));
    sensor->latestValue = 0;
    sensor->hasNewData = false;
    listOfSensors.append(sensor);
}


void SensorMgr::slot_SensorRemoved(int usbPort, QString name)
{
    qDebug() << Q_FUNC_INFO << " usbPort = " << usbPort
             << " name = " << name;
    sharedMemMgr.removeSensor(usbPort);
    for (int i =0; i < listOfSensors.length(); i++)
    {
        SensorReader * t = listOfSensors.at(i)->reader;
        if (t->getId() == usbPort)
        {
            t->terminate();
        }
        listOfSensors.removeAt(i);
        t->terminate();
        connect(t, SIGNAL(finished()),
                this, SLOT(slot_ReaderFinished()));
        needToDeleteThread= t;
    }

}

void SensorMgr::slot_ReaderFinished(SensorReader *t)
{
    qDebug() << "Delete " << t;
    delete t;
}

void SensorMgr::slot_ReaderFinished()
{
    qDebug() << Q_FUNC_INFO << "Delete " << needToDeleteThread;
//    delete needToDeleteThread;
    needToDeleteThread = 0;
}

void SensorMgr::slot_NewData(int id, float value)
{
//    qDebug() << Q_FUNC_INFO << " " << id << " " << value;
    Sensor_t * sensor = 0;
    for (int i =0; i < listOfSensors.length(); i++)
    {
        if (listOfSensors.at(i)->reader->getId() == id)
        {
            sensor = listOfSensors.at(i);
            break;
        }
    }

    if (sensor)
    {
        sensor->latestValue = value;
        if (!sensor->hasNewData)
        {
            sensor->hasNewData = true;
            /** check if enough data */
            for (int i =0; i < listOfSensors.length(); i++)
            {
                if (!listOfSensors.at(i)->hasNewData)
                    return;
            }

            /** Add a record */
            Record r;
            r.recordID = currentRecordID++;
            for (int i =0; i < listOfSensors.length(); i++)
            {
                sensor = listOfSensors.at(i);
                sensor->hasNewData = false;
                r.value[sensor->reader->getId()] = sensor->latestValue;
            }
            sharedMemMgr.addRecord(r);
            emit sng_recordAdded();

        }
    }
}
