#ifndef SERVERWEB_H
#define SERVERWEB_H

//#include "threadWeb.h"
#include <QMutex>
#include "QtWebSockets/QWebSocketServer"


QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class ServerWeb : public QObject
{
    Q_OBJECT
public:
    explicit ServerWeb(quint16 port, QObject *parent = Q_NULLPTR);
    virtual ~ServerWeb();

private Q_SLOTS:
    void onNewConnection();
    void processMessage(QString message);
    void socketDisconnected();



    void createGame(QWebSocket *socketWeb, QByteArray peerPort, QByteArray Data);


private:
    QWebSocketServer *m_pWebSocketServer;
    QMap<QByteArray, QWebSocket* > clients;
    QMap<QByteArray, QList<QByteArray> > rooms;
    QMap<QByteArray, QList<QByteArray> > initMap();

    QMap<QByteArray, QWebSocket* > servers;
    QMap<QByteArray, QByteArray > initServers();

    QMap<QByteArray, QList<QByteArray> > clientsInfo;
    QMap<QByteArray, QList<QByteArray> > initClientsInfo();

    QByteArray data_rooms(QByteArray peerport);
    QByteArray data_statsUser(QByteArray peerport);
    QByteArray data_statsRoom(QByteArray name);
    void data_roomsAll();
    void connectSocket(QWebSocket *socketWeb);
    void connectRoom(QWebSocket *socketWeb, QByteArray peerPort, QByteArray Data);
    void connectGame(QWebSocket *socketWeb, QByteArray peerPort, QByteArray data);
    void socketDis(QWebSocket *socketWeb);
};

#endif // SERVERWEB_H
