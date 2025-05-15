#include "UdpChatSocketManager.h"
#include <QNetworkDatagram>
#include <QStringList>
#include <QDateTime>

UdpChatSocketManager::UdpChatSocketManager(QObject *parent)
    : QObject(parent)
{}

UdpChatSocketManager::~UdpChatSocketManager()
{
    closeSockets();
}

bool UdpChatSocketManager::bindSendSocket(const QHostAddress &localAddress)
{
    if (sendSocket) {
        delete sendSocket;
        sendSocket = nullptr;
    }

    sendSocket = new QUdpSocket(this);
    return sendSocket->bind(localAddress, 0);
}

bool UdpChatSocketManager::bindReceiveSocket(const QHostAddress &localAddress, const QHostAddress &groupAddress, quint16 port)
{
    if (recvSocket) {
        delete recvSocket;
        recvSocket = nullptr;
    }

    recvSocket = new QUdpSocket(this);
    if (!recvSocket->bind(localAddress, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        return false;
    }

    if (groupAddress.isMulticast()) {
        recvSocket->joinMulticastGroup(groupAddress);
    }

    connect(recvSocket, &QUdpSocket::readyRead, this, &UdpChatSocketManager::processPendingDatagrams);
    return true;
}

qint64 UdpChatSocketManager::sendMessage(const QByteArray &data, const QHostAddress &targetAddress, quint16 targetPort)
{
    if (!sendSocket) return -1;

    lastSentData = data;
    return sendSocket->writeDatagram(data, targetAddress, targetPort);
}

void UdpChatSocketManager::closeSockets()
{
    if (recvSocket) {
        recvSocket->close();
        delete recvSocket;
        recvSocket = nullptr;
    }
    if (sendSocket) {
        sendSocket->close();
        delete sendSocket;
        sendSocket = nullptr;
    }
}

void UdpChatSocketManager::setLoopbackMode(bool enabled)
{
    loopbackEnabled = enabled;
}

void UdpChatSocketManager::setMulticastMode(bool enabled)
{
    multicastEnabled = enabled;
}

void UdpChatSocketManager::processPendingDatagrams()
{
    if (!recvSocket) return;

    while (recvSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(recvSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 senderPort;
        recvSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (datagram == lastSentData) {
            lastSentData.clear();
            continue; // Ignore self-sent echo
        }

        QString receivedText = QString::fromUtf8(datagram);
        QStringList parts = receivedText.split(" - ", Qt::KeepEmptyParts);
        if (parts.size() >= 2) {
            QString user = parts[0].trimmed();
            QString message = parts.mid(1).join(" - ").trimmed();
            emit messageReceived(user, message);
        } else {
            emit messageReceived("Unknown", receivedText);
        }
    }
}

QString UdpChatSocketManager::lastError() const {
    return sendSocket ? sendSocket->errorString() : "No socket";
}
