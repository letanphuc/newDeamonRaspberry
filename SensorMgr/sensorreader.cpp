#include "sensorreader.h"
#include <QSerialPort>
#include <QDebug>


SensorReader::SensorReader(QObject *parent, int id, QString devName):
    QThread(parent),
    devName(devName),
    id(id),
    isStop(false)
{
    start();
}


void SensorReader::run()
{
    QSerialPort serial;
    serial.setBaudRate(115200);
    serial.setPortName(devName);

    if (!serial.open(QIODevice::ReadWrite))
    {
        qDebug() << "Can not open port: " << devName;
        return;
    }

    while (isRunning())
    {
        if (isStop) break;

        serial.waitForReadyRead(10);
        QByteArray arr = serial.readAll();
        if (arr.length() > 0)
        {
            qDebug() << Q_FUNC_INFO << " " << QString::fromLatin1(arr);
        }
    }

    emit sgn_Finished(this);
}

void SensorReader::stop()
{
    isStop = true;
}

int SensorReader::getId() const
{
    return id;
}

void SensorReader::setId(int value)
{
    id = value;
}
