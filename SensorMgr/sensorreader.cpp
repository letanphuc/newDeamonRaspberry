#include "sensorreader.h"
#include <QSerialPort>
#include <QDebug>


SensorReader::SensorReader(QObject *parent, int id, QString devName):
    QThread(parent),
    devName(devName),
    id(id),
    isStop(false)
{
    qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
    start();
}


void SensorReader::run()
{
    QSerialPort serial;
    QByteArray arr;
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

        serial.waitForReadyRead(-1);
        arr += serial.readAll();
        if (arr.length() > 0)
        {
            handleData(arr);
        }
    }
    qDebug() << Q_FUNC_INFO << " End";

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

void SensorReader::handleData(QByteArray &arr)
{
    if (arr.length() >= 4){
        /** Read 4 byte */
        QByteArray number = arr.left(4);
        arr.remove(0, 4);

        float * data = (float*) number.data();
        qDebug() << "Value = " << *data;

        sgn_NewData(id, *data);
    }
}

void SensorReader::slot_Error(QSerialPort::SerialPortError error)
{
    qDebug() << Q_FUNC_INFO << " error = " << (int) error;
}
