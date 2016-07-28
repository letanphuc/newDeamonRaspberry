#include <QCoreApplication>
#include "TCPServer/tcpserver.h"
#include "SensorMgr/sensormgr.h"
#include "WebsocketServer/websocketserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPServer tcp;
    WebSocketServer websocket;
    sensorMgr.start();


    Q_UNUSED(tcp);
    Q_UNUSED(websocket);

    return a.exec();
}

