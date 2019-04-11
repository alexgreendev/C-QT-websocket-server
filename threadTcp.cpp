// mythread.cpp

#include "threadTcp.h"
#include "serverTcp.h"
#include "global.h"

ThreadTcp::ThreadTcp(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void ThreadTcp::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    Singleton::connectTcpSocket(socket);

    exec();
}

void ThreadTcp::data_roomsTcp(QTcpSocket *socket)
{
        QString ar_rooms;
        QMapIterator<QByteArray,QList<QByteArray> > iter(Singleton::rooms);

        while(iter.hasNext()) {
            iter.next();

            if(iter.key() == "header_room") continue;
            if(Singleton::rooms[iter.key()].count())
            {
                QVariant count = Singleton::rooms[iter.key()].count();
                ar_rooms.append(";"+iter.key()+","+count.toString());
            }
        }
        QString request = "24"+ar_rooms;
        qDebug() << ar_rooms;
        socket->write(request.toUtf8());
}

void ThreadTcp::readyRead()
{
    QByteArray Data = socket->readAll();
    QString request;

    if(Data[0] == '1')
    {
        //Пересылка синхроданных в комнате
        if(Data[1] == '1')
        {
            Data.remove(0,2);
            QVariant peerPort = socket->peerPort();
            request = "11{\"sync\":"+Data+", \"property\":{\"id\":"+peerPort.toString()+"}}";
            Q_FOREACH (QByteArray pClient, Singleton::rooms[socket->property("id_room").toByteArray()])
            {
                if(pClient[0] == '#')
                {
                    if('#'+peerPort.toByteArray() != pClient)
                    {
                        Singleton::clientsTcp[pClient]->write(request.toUtf8());
                    }
                }
                else
                {
                    Singleton::clientsWeb[pClient]->sendTextMessage(request.toUtf8());
                }
            }
        }
    }
    if(Data[0] == '2')
    {
        if(Data[1] == '1')
        {
            Data.remove(0,2);
            QVariant peerPort = socket->peerPort();
            Q_FOREACH (QByteArray pClient, Singleton::rooms[socket->property("id_room").toByteArray()])
            {
                if(pClient[0] == '#')
                {
                    if('#'+peerPort.toByteArray() != pClient)
                    {
                        Singleton::clientsTcp[pClient]->write(Data);
                    }
                }
                else
                {
                    Singleton::clientsWeb[pClient]->sendTextMessage(Data);
                }
            }
        }
        if(Data[0] == '2')
        {

        }
        if(Data[0] == '3')
        {

        }
        if(Data[0] == '4')
        {

        }
    }
    /*
    if(Data[0] == '2')
    {
        // сообщение в чате
        if(Data[1] == '1')
        {
            Q_FOREACH (QString *pClient, Singleton::rooms["header_room"])
            {
                pClient->write(Data);
            }
            return;
        }
        // Создать игру
        if(Data[1] == '2')
        {
            if(socket->property("id_room") == "header_room")
            {
                Singleton::createGameTcp(socket, Data);
                return;
            }
        }
        // Войти в игру
        if(Data[1] == '3')
        {
            QVariant peerPortt = socket->peerPort();
            Singleton::rooms[socket->property("id_room").toString()].removeOne(peerPortt.toString());
            socket->setProperty("id_room", Data.remove(0,2));
            Singleton::rooms[Data] << peerPortt.toString();
            QString peersPorts;
            Q_FOREACH (QString *pClient, Singleton::rooms[socket->property("id_room").toString()])
            {
                qDebug() << "!!!!! " << pClient->peerPort();
                if(socket != pClient)
                {
                    QVariant peerPort = socket->peerPort();
                    QVariant peerPorts = pClient->peerPort();
                    peersPorts.append(peerPorts.toString()+",");
                    request = "23+"+peerPort.toString();
                    pClient->write(request.toUtf8());
                }
                else
                {
                    request = "23"+peersPorts;
                    pClient->write(request.toUtf8());
                }
            }
            return;
        }
        //Список игр
        if(Data[1] == '4')
        {
            QString ar_rooms;
            QMapIterator<QString,QList<QString> > iter(Singleton::rooms);

            while(iter.hasNext()) {
                iter.next();

                if(iter.key() == 0) continue;
                if(iter.key() == "header_room") continue;
                if(Singleton::rooms[iter.key()].count())
                {
                    QVariant count = Singleton::rooms[iter.key()].count();
                    ar_rooms.append(";"+iter.key()+","+count.toString());
                }
            }
            QString request = "24"+ar_rooms;
            qDebug() << ar_rooms;
            socket->write(request.toUtf8());
            return;
        }
    }*/

}

void ThreadTcp::disconnected()
{
    qDebug() << " Disconnected  " << socket->peerPort();
    QString request;
    if (socket)
    {/*
        QVariant peerPortt = socket->peerPort();
        Singleton::rooms[socket->property("id_room").toString()].removeOne(peerPortt.toString());

        Q_FOREACH (QString *pClient, Singleton::rooms[socket->property("id_room").toString()])
        {
            QVariant peerPort = socket->peerPort();
            request = "15"+peerPort.toString();
            pClient->write(request.toUtf8());
        }
        if(Singleton::rooms[socket->property("id_room").toString()].count() == 0)
        {
            Singleton::rooms.remove(socket->property("id_room").toString());
            //qDebug() << "socketDisconnected" << pClient->property("id_room").toString();
        }
        socket->deleteLater();*/
    }
    exit(0);
}
