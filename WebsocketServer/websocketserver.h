#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    WebSocketServer(QObject *parent = 0);
    ~WebSocketServer();

    static const int WEB_PORT;
    static const QString GET_DATA_CMD;
    static const QString GET_SENSOR_CMD;

signals:

public slots:
    void slot_HandleNewConnection();
    void slot_ReceiveMessage(QString message);
    void slot_HandleDisconnectSocket();

private:
    QWebSocketServer * webSocketServer;
    QList< QWebSocket* > listOfClients;
};

#endif // WEBSOCKETSERVER_H
