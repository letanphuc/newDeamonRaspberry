QT += core network gui sql websockets serialport

TARGET = qtDeamonRasp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    SensorMgr/sensormgr.cpp \
    SharedMemMgr/sharedmemmgr.cpp \
    TCPServer/tcpserver.cpp \
    TCPServer/databasemgr.cpp \
    WebsocketServer/websocketserver.cpp \
    SensorMgr/dedevicemonitor.cpp \
    SensorMgr/sensorreader.cpp

HEADERS += \
    SensorMgr/sensormgr.h \
    SharedMemMgr/sharedmemmgr.h \
    TCPServer/tcpserver.h \
    TCPServer/databasemgr.h \
    WebsocketServer/websocketserver.h \
    SensorMgr/devicemonitor.h \
    SensorMgr/sensorreader.h

