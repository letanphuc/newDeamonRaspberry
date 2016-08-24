#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QSerialPort>



typedef struct {
    double q; //process noise covariance
    double r; //measurement noise covariance
    double x; //value
    double p; //estimation error covariance
    double k; //kalman gain
} kalman_state;


class SensorReader : public QThread
{
    Q_OBJECT
public:
    SensorReader(QObject *parent = 0, int id = -1, QString devName = "");
    void run();
    void stop();

    int getId() const;
    void setId(int value);
    void handleData(QByteArray & arr);

private:
    QString devName;
    int id;
    bool isStop;
    kalman_state kalman;

signals:
    void sgn_Finished(SensorReader *);
    void sgn_NewData(int id, float value);
public slots:
    void slot_Error(QSerialPort::SerialPortError error);
};

#endif // SENSORREADER_H
