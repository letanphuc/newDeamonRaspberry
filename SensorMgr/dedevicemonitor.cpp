#include "devicemonitor.h"

int DeviceMonitor::getPortID(QString path)
{
    int id = 100;
    QMap<QString, int>::iterator i;
    for (i = listOfUSBPort.begin(); i != listOfUSBPort.end(); ++i)
    {
        if (i.key() == path)
        {
            id = i.value();
            break;
        }
    }

    return id;
}

DeviceMonitor::DeviceMonitor(QObject *parent):
    QObject(parent)
{
    setUp();
    currentEvent = NULL;
    listOfUSBPort["2-1.2"] = 1;
    listOfUSBPort["1-1.2"] = 2;
    listOfUSBPort["2-1.1"] = 3;
    registeredSUBSYSTEM.append("tty");
}

void DeviceMonitor::setUp()
{
    struct sockaddr_nl snl;
    int retval = 0;
    netlink_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (netlink_socket == -1) {
        qWarning("error getting socket");
        return;
    }
    memset(&snl, 0, sizeof(snl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    retval = bind(netlink_socket, (struct sockaddr*) &snl, sizeof(struct sockaddr_nl));
    if (retval < 0)
    {
        qWarning("bind failed: ");
        close(netlink_socket);
        netlink_socket = -1;
        return;
    }
    else if (retval == 0)
    {
        struct sockaddr_nl _snl;
        socklen_t _addrlen = sizeof(struct sockaddr_nl);
        retval = getsockname(netlink_socket, (struct sockaddr *)&_snl, &_addrlen);
        if (retval == 0)
            snl.nl_pid = _snl.nl_pid;
    }

    socketNotifier = new QSocketNotifier(netlink_socket, QSocketNotifier::Read, this);
    connect(socketNotifier, SIGNAL(activated(int)), SLOT(parseDeviceInfo()));
    socketNotifier->setEnabled(true);
}


static QString shortPath(QString path)
{
    QStringList list = path.split("/");
    return list.at(6);
}

void DeviceMonitor::endParseLine()
{
    if (currentEvent)
    {
        if (currentEvent->infos["ACTION"] == "add")
        {
            if (registeredSUBSYSTEM.contains(currentEvent->infos["SUBSYSTEM"]))
            {
                int id = getPortID(shortPath(currentEvent->infos["DEVPATH"]));
                qDebug() << "devideAdded " << id << " " << currentEvent->infos["DEVNAME"];
                emit devideAdded(id, currentEvent->infos["DEVNAME"]);
            }
        }
        else if (currentEvent->infos["ACTION"] == "remove")
        {
            if (registeredSUBSYSTEM.contains(currentEvent->infos["SUBSYSTEM"]))
            {
                int id = getPortID(shortPath(currentEvent->infos["DEVPATH"]));
                qDebug() << "devideRemoved " << id << " " << currentEvent->infos["DEVNAME"];
                emit devideRemoved(id, currentEvent->infos["DEVNAME"]);
            }
        }
        delete currentEvent;
        currentEvent = NULL;
    }
}

void DeviceMonitor::parseLine(QByteArray &line)
{
    QString sLine = QString::fromLatin1(line);
    if (sLine.contains('@'))
    {
        /** start new event */
        if (currentEvent)
        {
            endParseLine();
        }
        currentEvent = new EventInfos();
        currentEvent->str = sLine;

    }
    else
    {
        /** a infos line */
        if (!currentEvent)
        {
            currentEvent = new EventInfos();
            currentEvent->str = "emty";
        }
        QRegExp rx("(\\\"|\\=)");
        QStringList l = sLine.split(rx);
        if (l.length() >= 2)
        {
            currentEvent->infos[l.at(0)] = l.at(1);
        }
    }
}

void DeviceMonitor::parseDeviceInfo()
{
#define UEVENT_BUFFER_SIZE 1024
    QByteArray data;
    data.resize(UEVENT_BUFFER_SIZE*2);
    data.fill(0);
    size_t len = read(socketNotifier->socket(), data.data(), UEVENT_BUFFER_SIZE*2);
    data.resize(len);
    data = data.replace(0, '\n').trimmed();
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    while(!buffer.atEnd()) {
        QByteArray x = buffer.readLine().trimmed();
        parseLine(x);
    }
    endParseLine();
    buffer.close();
}
