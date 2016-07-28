#include <QCoreApplication>
#include "TCPServer/tcpserver.h"
#include "SensorMgr/sensormgr.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPServer tcp;
    sensorMgr.start();

    Q_UNUSED(tcp);

    return a.exec();
}

