#include "sensorreader.h"
#include <QSerialPort>
#include <QDebug>

kalman_state kalman_init(double q, double r, double p, double intial_value)
{
    kalman_state result;
    result.q = q;
    result.r = r;
    result.p = p;
    result.x = intial_value;

    return result;
}

double kalman_update(kalman_state* state, double measurement)
{
    //prediction update
    //omit x = x
    state->p = state->p + state->q;

    //measurement update
    state->k = state->p / (state->p + state->r);
    state->x = state->x + state->k * (measurement - state->x);
    state->p = (1 - state->k) * state->p;

    return state->x;
}


SensorReader::SensorReader(QObject *parent, int id, QString devName):
    QThread(parent),
    devName(devName),
    id(id),
    isStop(false),
    kalman(kalman_init(1, 15, 1, 0))
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

        sgn_NewData(id, (float)kalman_update(&kalman, *data));
    }
}

void SensorReader::slot_Error(QSerialPort::SerialPortError error)
{
    qDebug() << Q_FUNC_INFO << " error = " << (int) error;
}
