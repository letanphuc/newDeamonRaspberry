#include "sharedmemmgr.h"



QList<Record> SharedMemMgr::getRecordFromID(int id)
{
    mutex.lock();
    if (id == -1)
    {
        mutex.unlock();
        return listOfRecords;
    }
    else
    {
        int i = 0;
        for (i = 0; i < listOfRecords.length(); i++)
        {
            if (listOfRecords.at(i).recordID > id)
                break;
        }
        QList<Record> ret = listOfRecords.mid(i);
        mutex.unlock();
        return ret;
    }

}

QList<Record> SharedMemMgr::getRecordFromTime(qint64 ms)
{
    mutex.lock();
    if (ms == -1)
    {
        mutex.unlock();
        return listOfRecords;
    }
    else
    {
        int i = 0;
        for (i = 0; i < listOfRecords.length(); i++)
        {
            if (listOfRecords.at(i).ms >= ms)
                break;
        }
        QList<Record> ret = listOfRecords.mid(i);
        mutex.unlock();
        return ret;
    }
}

QList<Sensor> SharedMemMgr::getListOfSensors() const
{
    return listOfSensors;
}

Record * SharedMemMgr::getLastRecord() const
{
    if (listOfRecords.length() > 0)
    {
        return (Record *) &(listOfRecords.at(listOfRecords.length() - 1));
    }
    else
    {
        return NULL;
    }
}

void SharedMemMgr::addSensor(Sensor sensor)
{
    mutex.lock();
    listOfSensors.append(sensor);
    mutex.unlock();
}

void SharedMemMgr::removeSensor(int sensorID)
{
    mutex.lock();
    for (int i = 0; i < listOfSensors.length(); i++)
    {
        if (listOfSensors.at(i).id == sensorID)
        {
            listOfSensors.removeAt(i);
            break;
        }
    }
    mutex.unlock();
}

void SharedMemMgr::addRecord(Record record)
{
    mutex.lock();
    if (listOfRecords.length() >= MAX_RECORD_SAVED)
    {
        listOfRecords.removeFirst();
    }

    listOfRecords.append(record);
    mutex.unlock();
}

SharedMemMgr sharedMemMgr;
qint64 startms = QDateTime::currentMSecsSinceEpoch();
