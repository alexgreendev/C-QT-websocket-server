// mythread.cpp

#include "threadWeb.h"
#include "serverWeb.h"
#include "global.h"

ThreadWeb::ThreadWeb(QObject *parent) :
    QThread(parent)
{
    //this->socket = socket;
}

void ThreadWeb::run()
{
    QWebSocket socket = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    // thread starts here
    qDebug() << "WebSocket Thread started";

    connect(socket, &QWebSocket::textMessageReceived, this, &ThreadWeb::readyRead);
    connect(socket, &QWebSocket::disconnected, this, &ThreadWeb::disconnected);

    Singleton::connectWebSocket(socket);

qDebug() << socket->peerPort();
    exec();
}


void ThreadWeb::readyRead(QString Data)
{
    qDebug() << Data;

QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
qDebug() << pSender->peerPort();
    QString request;
    if(Data[0] == '1')
    {
        //Пересылка синхроданных в комнате
        if(Data[1] == '1')
        {
            Q_FOREACH (QWebSocket *pClient, rooms[pSender->property("id_room").toString()])
            {
                if(pSender != pClient)
                {
                    QVariant peerPort = pSender->peerPort();
                    request = Data+"#"+peerPort.toString();
                    pClient->sendTextMessage(request.toUtf8());
                }
            }
        }
        if(Data[1] == '2')
        {
            Q_FOREACH (QWebSocket *pClient, rooms[pSender->property("id_room").toString()])
            {
                if(pSender != pClient)
                {
                    QVariant peerPort = pSender->peerPort();
                    request = Data+"#"+peerPort.toString();
                    pClient->sendTextMessage(request.toUtf8());
                }
            }
        }
    }/*
    if(Data[0] == '2')
    {
        // сообщение в чате
        if(Data[1] == '1')
        {
            Q_FOREACH (QWebSocket *pClient, rooms["header_room"])
            {
                pClient->sendTextMessage(Data);
            }
            return;
        }
        // Создать игру
        if(Data[1] == '2')
        {
            if(pSender->property("id_room") == "header_room")
            {
                //ServerWeb::createGameWeb(socket, Data);
                return;
            }
        }
        // Войти в игру
        if(Data[1] == '3')
        {
            rooms[pSender->property("id_room").toString()].removeOne(pSender);
            pSender->setProperty("id_room", Data.remove(0,2));
            rooms[Data] << pSender;
            QString peersPorts;
            Q_FOREACH (QWebSocket *pClient, rooms[pSender->property("id_room").toString()])
            {
                qDebug() << "!!!!! " << pClient->peerPort();
                if(pSender != pClient)
                {
                    QVariant peerPort = pSender->peerPort();
                    QVariant peerPorts = pClient->peerPort();
                    peersPorts.append(peerPorts.toString()+",");
                    request = "23+"+peerPort.toString();
                    pClient->sendTextMessage(request.toUtf8());
                }
                else
                {
                    request = "23"+peersPorts;
                    pClient->sendTextMessage(request.toUtf8());
                }
            }
            return;
        }
        //Список игр
        if(Data[1] == '4')
        {
            QString ar_rooms;
            QMapIterator<QString,QList<QWebSocket*> > iter(rooms);

            while(iter.hasNext()) {
                iter.next();

                if(iter.key() == 0) continue;
                if(iter.key() == "header_room") continue;
                if(rooms[iter.key()].count())
                {
                    QVariant count = rooms[iter.key()].count();
                    ar_rooms.append(";"+iter.key()+","+count.toString());
                }
            }
            QString request = "24"+ar_rooms;
            qDebug() << ar_rooms;
            pSender->sendTextMessage(request.toUtf8());
            return;
        }
    }*/

}

void ThreadWeb::disconnected()
{
    qDebug() << "WebSocket Disconnected  " << socket->peerPort();

    QString request;
    if (socket)
    {
        rooms[socket->property("id_room").toString()].removeOne(socket);

        Q_FOREACH (QWebSocket *pClient, rooms[socket->property("id_room").toString()])
        {
            QVariant peerPort = socket->peerPort();
            request = "15"+peerPort.toString();
            pClient->sendTextMessage(request.toUtf8());
        }
        if(rooms[socket->property("id_room").toString()].count() == 0)
        {
            rooms.remove(socket->property("id_room").toString());
        }
        socket->deleteLater();
    }
    exit(0);
}

