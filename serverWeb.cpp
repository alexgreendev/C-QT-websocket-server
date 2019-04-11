// myserver.cpp

#include "serverWeb.h"
#include "threadWeb.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include "QJsonDocument.h"
#include "QJsonObject.h"
#include "QJsonArray.h"

QT_USE_NAMESPACE


//! [constructor]
ServerWeb::ServerWeb(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(Q_NULLPTR)
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Chat Server"),
                                              QWebSocketServer::NonSecureMode,
                                              this);
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "WebSocketServer listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &ServerWeb::onNewConnection);
    }
}

ServerWeb::~ServerWeb()
{
    m_pWebSocketServer->close();
}
//! [constructor]
/*
QMap<QByteArray, QList<QWebSocket> > ServerWeb::initMap() {
    QMap<QByteArray, QList<QWebSocket> > rooms;
    return rooms;
}
QMap<QByteArray, QList<QWebSocket> > ServerWeb::rooms = initMap();
*/
void ServerWeb::data_roomsAll()
{
    QByteArray ar_rooms;
    QMapIterator<QByteArray,QList<QByteArray> > iter(rooms);

    ar_rooms.append("{");
    int i = 0;
    while(iter.hasNext()) {
        iter.next();

        if(iter.key() == "header_room") continue;
        if(rooms[iter.key()].count())
        {
            ar_rooms.append("\""+iter.key()+"\":{"+
                            "\"id\":"+"\""+iter.key()+"\","+
                            "\"count\":"+"\""+QByteArray::number(rooms[iter.key()].count()-3)+"\""+
                            +"},");
        }
        i++;
    }
    if(i > 0)
    {
        ar_rooms.remove(ar_rooms.length()-1, ar_rooms.length());
        ar_rooms.append(+"}");

        Q_FOREACH (QByteArray pClient, rooms["header_room"])
        {
            clients[pClient]->sendBinaryMessage("25"+ar_rooms);
        }
    }
    else
    {
         qDebug() << "data_roomsAll ";
        Q_FOREACH (QByteArray pClient, rooms["header_room"])
        {
            clients[pClient]->sendBinaryMessage("25null");
        }
    }

}
QByteArray ServerWeb::data_rooms(QByteArray peerport)
{
    QByteArray ar_rooms;
    QMapIterator<QByteArray,QList<QByteArray> > iter(rooms);

    int i = 0;
    ar_rooms.append("{\"idThis\":\""+peerport+"\",");
    while(iter.hasNext()) {
        iter.next();

        if(iter.key() == "header_room") continue;
        if(rooms[iter.key()].count())
        {
            ar_rooms.append("\""+iter.key()+"\":{"+
                            "\"id\":"+"\""+iter.key()+"\","+
                            "\"count\":"+"\""+QByteArray::number(rooms[iter.key()].count()-3)+"\""+
                            +"},");
        }
        i++;
    }
    ar_rooms.remove(ar_rooms.length()-1, ar_rooms.length());
    ar_rooms.append("}");

    return ar_rooms;


}
QByteArray ServerWeb::data_statsUser(QByteArray peerport)
{
    QByteArray res = "{";
    res.append("\"id\": \""+peerport+"\",");
    res.append( " \"nik\": \""+clientsInfo[peerport][2]+
            "\", \"active\": \""+clientsInfo[peerport][6]+
            "\", \"team\": \""+clientsInfo[peerport][7]+
            "\", \"die\": \""+clientsInfo[peerport][8]+
            "\", \"kill\": \""+clientsInfo[peerport][9]+"\"}");
    return res;
}
QByteArray ServerWeb::data_statsRoom(QByteArray name)
{
    if(rooms[name].count() < 1)
    {
        return "";
    }
    QByteArray res = "{";
    for(int i=3; i < rooms[name].count(); i++ )
    {
        QByteArray id = rooms[name][i];

        res.append("\"room\": {");
        res.append( " \"id\": \""+name+
                "\", \"lvl\": \""+rooms[name][1]+
                "\", \"mode\": \""+rooms[name][2]+"\"},");
        res.append("\""+id+"\": {");
        res.append( " \"nik\": \""+clientsInfo[id][2]+
                "\", \"active\": \""+clientsInfo[id][6]+
                "\", \"team\": \""+clientsInfo[id][7]+
                "\", \"die\": \""+clientsInfo[id][8]+
                "\", \"kill\": \""+clientsInfo[id][9]+
                "\", \"id\": \""+id+"\"},");
    }
    res.remove(res.length()-1, res.length());
    res.append(+"}");
    return res;
}

void ServerWeb::connectSocket(QWebSocket *socketWeb)
{
    QByteArray peerPort = QByteArray::number(socketWeb->peerPort());
    clients[peerPort] = socketWeb;
    socketWeb->setProperty("id_room", "header_room");

    QByteArray dRoom = data_rooms(peerPort);
    rooms["header_room"] << peerPort;
    qDebug() << "Socket Client connected";
    qDebug() << peerPort;

    socketWeb->sendBinaryMessage(dRoom.prepend("0"));
}

void ServerWeb::createGame(QWebSocket *socketWeb, QByteArray peerPort, QByteArray Data)
{
    QByteArray request;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(Data.remove(0,2));
    QJsonObject jsonObject = jsonResponse.object();

    if(!jsonObject["name"].isNull() && !jsonObject["mode"].isNull() && !jsonObject["lvl"].isNull())
    {
        if(!rooms.contains(jsonObject["name"].toString().toUtf8()))
        {
            socketWeb->setProperty("server", true);
            socketWeb->setProperty("id_room", jsonObject["name"].toString().toUtf8());

            rooms["header_room"].removeOne(peerPort);
            rooms[jsonObject["name"].toString().toUtf8()] << peerPort;
            rooms[jsonObject["name"].toString().toUtf8()] << jsonObject["lvl"].toString().toUtf8();
            rooms[jsonObject["name"].toString().toUtf8()] << jsonObject["mode"].toString().toUtf8();
            rooms[jsonObject["name"].toString().toUtf8()] << peerPort;
            request = "22"+peerPort;
            socketWeb->sendBinaryMessage(request);
            data_roomsAll();
        }
        else
        {
            socketWeb->sendBinaryMessage("221");
        }
    }
}
void ServerWeb::connectRoom(QWebSocket *socketWeb, QByteArray peerPort, QByteArray Data)
{
    if(rooms.contains(Data.remove(0,2)))
    {
        qDebug() << Data;
        socketWeb->setProperty("server", false);
        socketWeb->setProperty("id_room", Data);
        socketWeb->sendBinaryMessage(data_statsRoom(Data).prepend("23"));
        qDebug() << "1111111";
        rooms["header_room"].removeOne(peerPort);
        rooms[Data] << peerPort;
        QByteArray curUser = data_statsUser(peerPort);
        qDebug() << "2222222";
        curUser.prepend("23+");
        for(int i=3; i < rooms[Data].count(); i++ )
        {
            if(peerPort != rooms[Data][i])
            {
                clients[rooms[Data][i]]->sendBinaryMessage(curUser);
            }
        }

    }
    else
    {
        socketWeb->sendBinaryMessage("231");
    }
}
void ServerWeb::connectGame(QWebSocket *socketWeb, QByteArray peerPort, QByteArray data)
{
    QByteArray name_room = socketWeb->property("id_room").toByteArray();

    if(name_room != "header_room" && clientsInfo[peerPort][6] == "0")
    {
        clientsInfo[peerPort][6] = "1";
        qDebug() << "%%%%%  " << data;
        if(data == "241")
        {
            clientsInfo[peerPort][7] = "0";
        }
        if(data == "242")
        {
            clientsInfo[peerPort][7] = "1";
        }
        for(int i=3; i < rooms[name_room].count(); i++ )
        {
            if(peerPort != rooms[name_room][i])
            {
                clients[rooms[name_room][i]]->sendBinaryMessage(data+peerPort);
            }
        }
    }
    else
    {
        socketWeb->sendBinaryMessage("999");
    }
}

void ServerWeb::socketDis(QWebSocket *socketWeb)
{
    QByteArray request;
    QByteArray peerPort = QByteArray::number(socketWeb->peerPort());
    QByteArray name_room;
    bool coorServer = false;

    name_room = socketWeb->property("id_room").toByteArray();
    coorServer = socketWeb->property("server").toBool();

    qDebug() << "Disconnect " << peerPort;
    request = "15"+peerPort;
    QByteArray new_server;

    if(name_room != "header_room")
    {
        if(rooms[name_room].count() == 4)
        {
            rooms.remove(name_room);
            data_roomsAll();
        }
        else
        {
            if(coorServer)
            {
                if(peerPort != rooms[name_room][3])
                {
                    new_server = rooms[name_room][3];
                }
                else
                {
                    new_server = rooms[name_room][4];
                }
                rooms[name_room][0] = new_server;
                rooms[name_room].removeOne(peerPort);
                clients[new_server]->setProperty("server", true);
                clients[new_server]->sendBinaryMessage(peerPort.prepend("101"));
            }
            else
            {
                rooms[name_room].removeOne(peerPort);
            }
            for(int i=3; i < rooms[name_room].count(); i++ )
            {
                if(new_server != rooms[name_room][i])
                {
                    clients[rooms[name_room][i]]->sendBinaryMessage(request);
                }
            }

        }
    }
    else
    {
        rooms[name_room].removeOne(peerPort);
    }

    clients.remove(peerPort);
}


//! [onNewConnection]
void ServerWeb::onNewConnection()
{
    qDebug() << "QWebSocket connect..." ;

    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &ServerWeb::processMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ServerWeb::socketDisconnected);
    pSocket->setProperty("auth", false);
    connectSocket(pSocket);
}
//! [onNewConnection]

//! [processMessage]
void ServerWeb::processMessage(QString message)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    QString request;
    QByteArray peerPort = QByteArray::number(socket->peerPort());
    QByteArray name_room = socket->property("id_room").toByteArray();
    QByteArray server_room = rooms[name_room][0];
    if(socket->property("auth").toBool())
    {
        if(message[0] == '1')
        {
            if(name_room == "header_room")
            {
                return;
            }
            //Пересылка синхроданных в комнате
            if(message[1] == '1')
            {
                message.remove(0,2);
                request = "11{\"sync\":"+message+", \"property\":{\"id\":"+QString(peerPort)+"}}";

                for(int i=3; i < rooms[name_room].count(); i++ )
                {
                    if(peerPort != rooms[name_room][i])
                    clients[rooms[name_room][i]]->sendTextMessage(request);
                }
                return;

            }
            if(message[1] == '2')
            {
                message.remove(0,2);
                QByteArray dead = message.left(message.indexOf("/")).toUtf8();
                QByteArray killer = message.right(message.indexOf("/")).toUtf8();

                if(!clientsInfo.contains(dead) || !clientsInfo.contains(killer))
                {
                    return;
                }
                int deadI = clientsInfo[dead][7].toInt();
                deadI++;
                clientsInfo[dead][6] = "0";
                clientsInfo[dead][7] = QByteArray::number(deadI);
                int killerI = clientsInfo[killer][8].toInt();
                killerI++;
                clientsInfo[killer][8] = QByteArray::number(killerI);

                for(int i=3; i < rooms[name_room].count(); i++ )
                {
                    clients[rooms[name_room][i]]->sendTextMessage("12"+message);
                }
            }
            if(message[1] == '3')
            {

            }
            if(message[1] == '5')
            {


                for(int i=3; i < rooms[name_room].count(); i++ )
                {
                    clients[rooms[name_room][i]]->sendTextMessage(request);
                }
            }
        }
        if(message[0] == '2')
        {
            if(message[1] == '1')
            {
                int i=0;
                if(name_room != "header_room")
                {
                    i = 3;
                }
                while(i < rooms[name_room].count())
                {
                    clients[rooms[name_room][i]]->sendTextMessage(message);
                    i++;
                }
                return;
            }
            if(message[1] == '2')
            {
                createGame(socket, peerPort, message.toUtf8());
                return;
            }
            if(message[1] == '3')
            {
                connectRoom(socket, peerPort, message.toUtf8());
                return;
            }
            if(message[1] == '4')
            {
                connectGame(socket, peerPort, message.toUtf8());
                return;
            }
            if(message[1] == '5')
            {
                //data_rooms(socket);
                return;
            }
        }
    }
    else
    {
        if(message[0] == '3')
        {
            //Регистрация
            if(message[1] == '1')
            {
                QJsonDocument jsonResponse = QJsonDocument::fromJson(message.remove(0,2).toUtf8());
                QJsonObject jsonObject = jsonResponse.object();

                socket->setProperty("auth", true);

                clientsInfo[peerPort] << jsonObject["fname"].toVariant().toByteArray();
                clientsInfo[peerPort] << jsonObject["lname"].toVariant().toByteArray();
                clientsInfo[peerPort] << jsonObject["nik"].toVariant().toByteArray();
                clientsInfo[peerPort] << jsonObject["email"].toVariant().toByteArray();
                clientsInfo[peerPort] << "0"; //die all
                clientsInfo[peerPort] << "0"; //killer all
                clientsInfo[peerPort] << "0"; //active in room
                clientsInfo[peerPort] << "0"; //team in room
                clientsInfo[peerPort] << "0"; //die coor room
                clientsInfo[peerPort] << "0"; //killer coor room
                request = "311{\"nik\": \""+ QString(clientsInfo[peerPort][2]) +"\"}";
                socket->sendTextMessage(request.toUtf8());
                return;
            }
            //Авторизация
            if(message[1] == '2')
            {
                QJsonDocument jsonResponse = QJsonDocument::fromJson(message.remove(0,2).toUtf8());
                QJsonObject jsonObject = jsonResponse.object();

                if(!jsonObject["login"].isUndefined() && !jsonObject["pass"].isUndefined())
                {
                    socket->setProperty("auth", true);

                    clientsInfo[peerPort] << "TestName";
                    clientsInfo[peerPort] << "TestPhamily";
                    clientsInfo[peerPort] << "ТестыNik";
                    clientsInfo[peerPort] << "TestEmail";
                    clientsInfo[peerPort] << "0"; //die all
                    clientsInfo[peerPort] << "0"; //killer all
                    clientsInfo[peerPort] << "0"; //active in room
                    clientsInfo[peerPort] << "0"; //team in room
                    clientsInfo[peerPort] << "0"; //die coor room
                    clientsInfo[peerPort] << "0"; //killer coor room
                    request = "321{\"nik\": \""+ QString(clientsInfo[peerPort][2]) +"\"}";
                    socket->sendBinaryMessage(request.toUtf8());
                }
                return;
            }
        }
        else
        {
            qDebug() << "auth error " << QByteArray::number(socket->peerPort());
            request = "30error";
            socket->sendBinaryMessage(request.toUtf8());
        }
    }

}


//! [processMessage]

//! [socketDisconnected]
void ServerWeb::socketDisconnected()
{

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    qDebug() << "QWebSocket Client disconnect";
    if (pClient)
    {
        socketDis(pClient);
        pClient->deleteLater();
    }
}
//! [socketDisconnected]
