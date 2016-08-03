#include "websocketserver.h"
#include <QStringList>
#include <QDebug>
#include "SharedMemMgr/sharedmemmgr.h"

const int WebSocketServer::WEB_PORT = 5439;
const QString WebSocketServer::GET_DATA_CMD = "getdata";
const QString WebSocketServer::GET_SENSOR_CMD = "sensor";

WebSocketServer::WebSocketServer(QObject *parent) :
    QObject(parent),
    webSocketServer(new QWebSocketServer(QStringLiteral("Data Server"),
                                         QWebSocketServer::NonSecureMode,
                                         this)),
    listOfClients()
{
    if (webSocketServer->listen(QHostAddress::Any, WEB_PORT))
    {
        qDebug() << "WebSocketServer listen on port: " << WEB_PORT;
        connect(webSocketServer, &QWebSocketServer::newConnection,
                this, &WebSocketServer::slot_HandleNewConnection);
    }
    else
    {
        qDebug() << "WebSocketServer can not listen on port: " << WEB_PORT;
        exit(-2);
    }

}

WebSocketServer::~WebSocketServer()
{
    webSocketServer->close();
    qDeleteAll(listOfClients.begin(), listOfClients.end());
}


void WebSocketServer::slot_HandleNewConnection()
{
    QWebSocket *socket = webSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketServer::slot_ReceiveMessage);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketServer::slot_HandleDisconnectSocket);

    listOfClients << socket;
}


void WebSocketServer::slot_ReceiveMessage(QString message)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    QString reply = "";
    if (message.startsWith(GET_DATA_CMD))
    {
        QStringList list = message.split("|");
        if (list.length() < 2)
        {
            reply = "|";
        }
        else
        {
            qint64 lastRecord = list[1].toInt();
            QList<Record> records = sharedMemMgr.getRecordFromTime(lastRecord);
            int i = records.length() - 1;
            if (i >= 0)
            {
                reply += QString::number(records.at(i).recordID) + "," +
                        QString::number(records.at(i).ms) + "," +
                        QString::number(records.at(i).value[0]) + "," +
                        QString::number(records.at(i).value[1]) + "," +
                        QString::number(records.at(i).value[2]) + "," +
                        QString::number(records.at(i).value[3]) + "|";

            }
        }
    }
    else if (message.startsWith(GET_SENSOR_CMD))
    {
        QList<Sensor> sensors = sharedMemMgr.getListOfSensors();
        for (int i = 0; i < sensors.length(); i ++ )
        {
            reply += QString::number(sensors.at(i).id) + "," +
                    sensors.at(i).type + "|";
        }

    }
    else
    {
        qDebug() << Q_FUNC_INFO << ": Unknown message";
        reply = "Unknown message";
    }
    pSender->sendTextMessage(reply);
}

void WebSocketServer::slot_HandleDisconnectSocket()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        listOfClients.removeAll(pClient);
        pClient->deleteLater();
    }
}
