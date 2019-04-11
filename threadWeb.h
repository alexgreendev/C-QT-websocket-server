#ifndef THREADWEB_H
#define THREADWEB_H

#include <QThread>
#include "QtWebSockets/QWebSocket"
#include <QDebug>
#include <QtCore/QMap>

class ThreadWeb : public QThread
{
    Q_OBJECT
public:
    explicit ThreadWeb(QObject *parent = 0);
    void run();

signals:
    //void error(QWebSocket:: socketerror);

public slots:
    void readyRead(QString Data);
    void disconnected();

public:
    //QWebSocket *socket;
    qintptr socketDescriptor;
    QMap<QString, QList<QWebSocket*> > rooms;
};

#endif // THREADWEB_H
