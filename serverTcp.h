// myserver.h

#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include "threadTcp.h"
 #include <QMutex>

class ServerTcp : public QObject
{
    Q_OBJECT
public:
    explicit ServerTcp(quint16 port, QObject *parent = 0);
    virtual ~ServerTcp();
signals:

public slots:
    void newConnection();
    void processMessage();
    void socketDisconnected();
    void data_rooms(QTcpSocket *pSender);

public:
     QTcpServer *server;
};
#endif // MYSERVER_H
