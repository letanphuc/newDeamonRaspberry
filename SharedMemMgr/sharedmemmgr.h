#ifndef SHAREDMEMMGR_H
#define SHAREDMEMMGR_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QQueue>
#include <QMutex>

#define MAX_SENSOR_SUPPORT  4
#define MAX_RECORD_SAVED    10

extern qint64 startms;

class Sensor{
public:
    int id;
    QString type;
    QString decription;
    QString devName;
    QString unit;
    Sensor(): id(0), type(""), decription(""){}
};

class Record{
public:
    int recordID;
    qint64 ms;
    double value[MAX_SENSOR_SUPPORT];
    Record():
        ms(QDateTime::currentMSecsSinceEpoch() - startms)
    {
        memset(value, 0, sizeof(value));
    }
};

class SharedMemMgr: public QObject
{
    Q_OBJECT
public:
    SharedMemMgr() {}

    /** For Database and websocket get data */
    QList<Record> getRecordFromID(int id);
    QList<Record> getRecordFromTime(qint64 ms);
    QList<Sensor> getListOfSensors() const;
    Record * getLastRecord() const;

    /** For sensor thread to update data */
    void addSensor(Sensor sensor);
    void removeSensor(int sensorID);
    Sensor getSensorInfo(int sensorID);
    void modifySensor(int sensorID, Sensor sensor);

    void addRecord(Record record);

private:
    QList<Sensor> listOfSensors;
    QList<Record> listOfRecords;
    QMutex mutex;



};

extern SharedMemMgr sharedMemMgr;

#endif // SHAREDMEMMGR_H
