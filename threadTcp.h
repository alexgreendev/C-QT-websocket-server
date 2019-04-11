// mythread.h

#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QtCore/QMap>

class ThreadTcp : public QThread
{
    Q_OBJECT
public:
    explicit ThreadTcp(qintptr ID, QObject *parent = 0);
    void run();
    void data_roomsTcp(QTcpSocket *socket);

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

public:
    QTcpSocket *socket;
    qintptr socketDescriptor;
};

#endif // MYTHREAD_H
