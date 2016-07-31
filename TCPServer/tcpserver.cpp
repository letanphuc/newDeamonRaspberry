#include <QtNetwork>
#include "tcpserver.h"
#include "databasemgr.h"

const QString TCPServer::CMD_START_RECORD = "start record";
const QString TCPServer::CMD_STOP_RECORD = "stop record";
const int TCPServer::TCP_PORT = 5438;

TCPServer::TCPServer():
    lastID(0),
    tcpServer(new QTcpServer(this)),
    clientConnection(NULL),
    listOfDatabase()
{
    if (!tcpServer->listen(QHostAddress::Any, TCP_PORT)) {
        qDebug() << "TCPServer can not listen on port " << TCP_PORT;
        exit(-1);
    }
    else {
        qDebug() << "TCPServer listen on " << tcpServer->serverPort();
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
        int id = (int) QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Start record id = " << id;
        reply = QString::number(lastID);
        DataBaseMgr *dbmgr = new DataBaseMgr(id, QString("/ram/") + QString::number(id) + ".db");
        listOfDatabase.append(dbmgr);

    }
    else if (str.startsWith(CMD_STOP_RECORD))
    {
        DataBaseMgr * dbmgr = NULL;
        if (str.contains('|'))
        {
            int id = str.split("|").at(1).toInt();
            qDebug() << "Stop record id = " << id;
            for (int i = 0; i < listOfDatabase.length(); i++)
            {
                if (listOfDatabase.at(i)->getId() == id)
                {
                    dbmgr = listOfDatabase.at(i);
                    listOfDatabase.removeAt(i);
                    break;
                }
            }
        }
        else if (listOfDatabase.length() > 0)
        {
            dbmgr = listOfDatabase.at(0);
            listOfDatabase.removeAt(0);
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
