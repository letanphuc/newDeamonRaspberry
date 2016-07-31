#ifndef DATABASEMGR_H
#define DATABASEMGR_H

#include <QSqlDatabase>
#include <QThread>
#include <QMutex>
#include "SharedMemMgr/sharedmemmgr.h"

class DataBaseMgr : public QThread
{
    Q_OBJECT

public:
    DataBaseMgr(int id, QString dataBasePath);
    ~DataBaseMgr();
    void stop();
    int getId() const;

    QString getDataBasePath() const;

    void run();

private:
    void createTable();
    void addSensorInfor();
    void addFirstRecord();
    void addARecord(Record & r);

    QString dataBasePath;
    QSqlDatabase db;
    int id;
    int lastRecordID;
    QMutex m_mutex;
    bool isStop;
    qint64 startTime;

signals:
    void done(DataBaseMgr * dbmgr);
public slots:
    void slot_NewRecordAdded();
};

#endif // DATABASEMGR_H
