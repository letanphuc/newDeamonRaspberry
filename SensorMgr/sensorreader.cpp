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
    kalman(kalman_init(1, 15, 1, 0)),
    currentState(IDLE)
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



    while (arr.length() > 0)
    {
        char b = arr.at(0);
        arr.remove(0, 1);

        switch (currentState)
        {
        case IDLE:
            if (b == 0x14)
            {
                currentState = REC_INFO;
                currentInfo.clear();
            }
            else if (b == 0x15)
            {
                currentState = REC_DATA;
                currentData.clear();
            }
            break;

        case REC_DATA:

            if (b == 0x15 && currentData.length() >= 4)
            {
                QByteArray number = currentData.left(4);
                arr.remove(0, 4);
                float * data = (float*) number.data();
                emit sgn_NewData(id, (float)kalman_update(&kalman, *data));

                currentState = IDLE;
            }
            else
            {
                currentData.push_back(b);
            }
            break;

        case REC_INFO:
            if (b == 0x14)
            {
                if (currentInfo.length() > 0)
                {
                    QString data = QString::fromLatin1(currentInfo.data());
                    QStringList tmp = data.split("|");
                    qDebug() << data;
                    if (data.length() >= 2)
                        emit sgn_NewInfoUpdate(id, tmp.at(0), tmp.at(1));
                }
                currentState = IDLE;
            }
            else
            {
                currentInfo.push_back(b);
            }
            break;
        }
    }

}

void SensorReader::slot_Error(QSerialPort::SerialPortError error)
{
    qDebug() << Q_FUNC_INFO << " error = " << (int) error;
}
