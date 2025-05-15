#ifndef UDPCHATSOCKETMANAGER_H
#define UDPCHATSOCKETMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class UdpChatSocketManager : public QObject
{
    Q_OBJECT

public:
    explicit UdpChatSocketManager(QObject *parent = nullptr);
    ~UdpChatSocketManager();

    bool bindReceiveSocket(const QHostAddress &localAddress, const QHostAddress &groupAddress, quint16 port);
    bool bindSendSocket(const QHostAddress &localAddress);
    qint64 sendMessage(const QByteArray &data, const QHostAddress &targetAddress, quint16 targetPort);
    void closeSockets();
    void setLoopbackMode(bool enabled);
    void setMulticastMode(bool enabled);

    QString lastError() const;
signals:
    void messageReceived(const QString &user, const QString &message);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *sendSocket = nullptr;
    QUdpSocket *recvSocket = nullptr;
    QByteArray lastSentData;
    bool multicastEnabled = false;
    bool loopbackEnabled = true;
};

#endif // UDPCHATSOCKETMANAGER_H
