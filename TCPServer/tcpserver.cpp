#include <QtNetwork>
#include "tcpserver.h"
#include "databasemgr.h"

const QString TCPServer::CMD_START_RECORD = "startrecord";
const QString TCPServer::CMD_STOP_RECORD = "stoprecord";
const int TCPServer::TCP_PORT = 8888;

TCPServer::TCPServer():
    lastID(0),
    tcpServer(new QTcpServer(this)),
    clientConnection(NULL),
    listOfDatabase()
{
    if (!tcpServer->listen(QHostAddress::Any, TCP_PORT)) {
        qDebug() << "Can not listen on port " << TCP_PORT;
    }
    else {
        qDebug() << "listen on " << tcpServer->serverPort();
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slot_NewConnection()));
    }
}

TCPServer::~TCPServer()
{
    tcpServer->close();
    delete tcpServer;
}

void TCPServer::slot_NewConnection()
{
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(slot_DataReceived()));
}

void TCPServer::slot_DataReceived()
{
    QString str = QString(clientConnection->readAll());
    qDebug() << Q_FUNC_INFO << str;
    QString reply = "";
    if (str.startsWith(CMD_START_RECORD))
    {
        lastID ++;
        qDebug() << "Start record id = " << lastID;
        reply = QString::number(lastID);
        DataBaseMgr *dbmgr = new DataBaseMgr(lastID, QString("/ram/") + QString::number(lastID) + ".db");
        listOfDatabase.append(dbmgr);

    }
    else if (str.startsWith(CMD_STOP_RECORD))
    {
        int id = str.split("|").at(1).toInt();
        qDebug() << "Stop record id = " << id;
        DataBaseMgr * dbmgr = NULL;
        for (int i = 0; i < listOfDatabase.length(); i++)
        {
            if (listOfDatabase.at(i)->getId() == id)
            {
                dbmgr = listOfDatabase.at(i);
                listOfDatabase.removeAt(i);
                break;
            }
        }
        if (dbmgr)
        {
            dbmgr->stop();
            qDebug() << "DB path is " << dbmgr->getDataBasePath();
            reply = dbmgr->getDataBasePath();
            connect(dbmgr, SIGNAL(done(DataBaseMgr*)),
                    this, SLOT(slot_HandleDataBaseDone(DataBaseMgr*)));
        }
        else
        {
            qDebug() << "No database found";
            reply = "No database found";
        }

    }

    clientConnection->write(reply.toLatin1());
    clientConnection->close();
}

void TCPServer::slot_HandleDataBaseDone(DataBaseMgr *dbmgr)
{
    qDebug() << "Delete " << (void *) dbmgr;
    delete dbmgr;
}
