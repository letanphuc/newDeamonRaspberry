#ifndef SENSORMGR_H
#define SENSORMGR_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "SharedMemMgr/sharedmemmgr.h"
#include "devicemonitor.h"
#include "sensorreader.h"

typedef struct {
    SensorReader * reader;
    bool hasNewData;
    float latestValue;
} Sensor_t;


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
    DeviceMonitor * deviceMonitor;
    QList<Sensor_t*> listOfSensors;
    SensorReader * needToDeleteThread;
    int currentRecordID;

signals:
    void sng_recordAdded();

public slots:
    void slot_SensorAdded(int usbPort, QString name);
    void slot_SensorRemoved(int usbPort, QString name);
    void slot_ReaderFinished(SensorReader * t);
    void slot_ReaderFinished();
    void slot_NewData(int id, float value);
    void slot_newInfoUpdate(int id, QString name, QString unit);

};


#endif // SENSORMGR_H
