#ifndef DEVICEMONITOR_H
#define DEVICEMONITOR_H

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>
#include <QDebug>
#include <QSocketNotifier>
#include <QByteArray>
#include <QBuffer>
#include <QMap>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>


class EventInfos: public QObject
{
    Q_OBJECT
public:
    QString str;
    QMap<QString, QString> infos;
};


class DeviceMonitor : public QObject
{
    Q_OBJECT
private:
    QSocketNotifier *socketNotifier;
    int netlink_socket;
    EventInfos * currentEvent;
    QStringList registeredSUBSYSTEM;
    QMap<QString, int> listOfUSBPort;

    int getPortID(QString path);

public:
    DeviceMonitor(QObject* parent = NULL);

    void setUp();

    void endParseLine();

    void parseLine(QByteArray & line);
signals:
    void devideAdded(int usbPort, QString devName);
    void devideRemoved(int usbPort, QString devName);

public slots:
    void parseDeviceInfo();
};

#endif // DEVICEMONITOR_H

