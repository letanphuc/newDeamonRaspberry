#ifndef SENSORMGR_H
#define SENSORMGR_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "SharedMemMgr/sharedmemmgr.h"
#include "devicemonitor.h"
#include "sensorreader.h"

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
    QList<SensorReader*> listOfReaderThread;

signals:
    void sng_recordAdded();

public slots:
    void slot_SensorAdded(int usbPort, QString name);
    void slot_SensorRemoved(int usbPort, QString name);
    void slot_ReaderFinished(SensorReader * t);

};


#endif // SENSORMGR_H
