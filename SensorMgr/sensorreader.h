#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>
#include <QThread>
#include <QString>

class SensorReader : public QThread
{
    Q_OBJECT
public:
    SensorReader(QObject *parent = 0, int id = -1, QString devName = "");
    void run();
    void stop();

    int getId() const;
    void setId(int value);

private:
    QString devName;
    int id;
    bool isStop;

signals:
    void sgn_Finished(SensorReader *);
public slots:
};

#endif // SENSORREADER_H
