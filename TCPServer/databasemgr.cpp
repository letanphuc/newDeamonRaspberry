#include <QDebug>
#include <QSqlQuery>
#include "databasemgr.h"
#include <QSqlError>



DataBaseMgr::DataBaseMgr(int id, QString dataBasePath):
    dataBasePath(dataBasePath),
    db(QSqlDatabase::addDatabase("QSQLITE", dataBasePath)),
    id(id),
    lastRecordID(0),
    isStop(false),
    startTime(QDateTime::currentMSecsSinceEpoch() - startms)
{
    start();
}

DataBaseMgr::~DataBaseMgr()
{

}

void DataBaseMgr::stop()
{
    m_mutex.lock();
    isStop = true;
    m_mutex.unlock();
}

int DataBaseMgr::getId() const
{
    return id;
}

QString DataBaseMgr::getDataBasePath() const
{
    return dataBasePath;
}

void DataBaseMgr::run()
{
    db.setDatabaseName(dataBasePath);
    if (!db.open())
    {
        qDebug() << "Cannot open database";
        return;
    }

    createTable();
    addSensorInfor();
    addFirstRecord();

    while (!isStop)
    {
        m_mutex.lock();
        if (isStop) break;
        m_mutex.unlock();

        QList<Record> list = sharedMemMgr.getRecordFromID(lastRecordID);
        if (list.length())
            qDebug() << "Add " << list.length() << " records";
        for (int i = 0; i < list.length(); i ++)
        {
            Record r = list.at(i);
            addARecord(r);
        }
        usleep(40);
    }

    db.close();

    emit done(this);
}

void DataBaseMgr::createTable()
{
    QSqlQuery query(db);
    static const QString CREATE_TABLE_DATA_CMD =
            "CREATE TABLE data (" \
            "	recordID	INTEGER," \
            "	recordTime	TEXT," \
            "	value1	REAL," \
            "	value2	REAL," \
            "	value3	REAL," \
            "	value4	REAL" \
            ")" ;
    static const QString CREATE_TABLE_DETAIL_CMD =
            "CREATE TABLE detail (" \
            "sensorID NUMERIC, " \
            "sensorName TEXT, " \
            "sensorType TEXT, " \
            "description TEXT" \
            ")" ;
    bool ret = query.exec(CREATE_TABLE_DATA_CMD);
    ret &= query.exec(CREATE_TABLE_DETAIL_CMD);

    Q_ASSERT(ret);
}

void DataBaseMgr::addSensorInfor()
{
    QList<Sensor> list = sharedMemMgr.getListOfSensors();
    QSqlQuery query(db);
    for (int i = 0; i < list.length(); i++)
    {
        Sensor s = list.at(i);
        QString cmd = QString("INSERT INTO \"detail\" VALUES(%1,'%2','%3','%4')")
                        .arg(s.id)
                        .arg(QString("name") + QString::number(s.id))
                        .arg(s.type)
                        .arg(s.decription);
        Q_ASSERT(query.exec(cmd));
    }
}

void DataBaseMgr::addFirstRecord()
{
    Record * r = sharedMemMgr.getLastRecord();
    if (r)
    {
        lastRecordID = r->recordID;
        startTime = r->ms;
        addARecord(*r);
    }
}

void DataBaseMgr::addARecord(Record &r)
{
    QSqlQuery query(db);
    r.ms -= startTime;
    QString cmd = QString("INSERT INTO data VALUES(%1,'%2',%3,%4,%5,%6)")
                    .arg(r.recordID)
                    .arg(QString::number(r.ms) + "ms")
                    .arg(QString::number(r.value[0]))
                    .arg(QString::number(r.value[1]))
                    .arg(QString::number(r.value[2]))
                    .arg(QString::number(r.value[3]));
    qDebug() << cmd;
    lastRecordID = r.recordID;
    Q_ASSERT(query.exec(cmd));
    qDebug() << query.lastError().text();
}

void DataBaseMgr::slot_NewRecordAdded()
{
    /** get last reocords in ShareMemMgr then save it */
}

