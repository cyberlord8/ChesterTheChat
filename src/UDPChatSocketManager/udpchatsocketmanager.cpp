/*
 * Chester The Chat
 * Copyright (C) 2024 - 2026 Timothy Millea
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "udpchatsocketmanager.h"

#include "../Utils/debugmacros.h"

#include <QNetworkDatagram>
#include <QStringList>
#include <QDateTime>

bool UdpChatSocketManager::isConnected() const {
    LOG_DEBUG("");

    return (sendSocket && sendSocket->state() == QAbstractSocket::BoundState) &&
           (recvSocket && recvSocket->state() == QAbstractSocket::BoundState);
}//isConnected

UdpChatSocketManager::UdpChatSocketManager(QObject *parent)
    : QObject(parent)
{
    LOG_DEBUG("");

}//UdpChatSocketManager

UdpChatSocketManager::~UdpChatSocketManager()
{
    LOG_DEBUG("");

    closeSockets();
}//UdpChatSocketManager

bool UdpChatSocketManager::bindSendSocket(const QHostAddress &localAddress)
{
    LOG_DEBUG("");

    if (sendSocket) {
        sendSocket->close();
        delete sendSocket;
        sendSocket = nullptr;
    }

    sendSocket = new QUdpSocket(this);

    const bool success = sendSocket->bind(localAddress, 0);

    sendSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 5);
    sendSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, loopbackEnabled ? 1 : 0);

    if (!success) {
        qWarning().nospace() << "[UdpChatSocketManager] Failed to bind send socket on "
                             << localAddress.toString() << ": " << sendSocket->errorString();
    }

    return success;
}//bindSendSocket

void UdpChatSocketManager::cleanupReceiveSocket()
{
    LOG_DEBUG("");

    if (recvSocket) {
        recvSocket->close();
        delete recvSocket;
        recvSocket = nullptr;
    }
}//cleanupReceiveSocket

bool UdpChatSocketManager::createAndBindReceiveSocket(const QHostAddress &localAddress,
                                                      const QHostAddress &groupAddress,
                                                      quint16 port)
{
    LOG_DEBUG("");

    recvSocket = new QUdpSocket(this);

    const bool multicast = groupAddress.isMulticast();
    const QHostAddress bindAddress =
        multicast ? QHostAddress::AnyIPv4 : localAddress;

    bool success = recvSocket->bind(
        bindAddress,
        port,
        QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    if (!success) {
        qCritical().nospace() << "[UdpChatSocketManager] Failed to bind receive socket on "
                              << localAddress.toString() << ":" << port
                              << " → " << recvSocket->errorString();
        delete recvSocket;
        recvSocket = nullptr;
    }

    return success;
}//createAndBindReceiveSocket

bool UdpChatSocketManager::joinMulticastGroupSafely(const QHostAddress &groupAddress)
{
    LOG_DEBUG("");

    if (!recvSocket->joinMulticastGroup(groupAddress)) {
        qWarning().nospace() << "[UdpChatSocketManager] Failed to join multicast group "
                             << groupAddress.toString() << ": " << recvSocket->errorString();
        return false;
    }
    return true;
}//joinMulticastGroupSafely

bool UdpChatSocketManager::bindReceiveSocket(const QHostAddress &localAddress,
                                             const QHostAddress &groupAddress,
                                             quint16 port)
{
    LOG_DEBUG("");

    cleanupReceiveSocket();

    if (!createAndBindReceiveSocket(localAddress, groupAddress, port))
        return false;

    if (groupAddress.isMulticast()){
        if(!joinMulticastGroupSafely(groupAddress)){
            cleanupReceiveSocket();
            return false;
        }
    }

    connect(recvSocket, &QUdpSocket::readyRead,
            this, &UdpChatSocketManager::processPendingDatagrams);

    return true;
}//bindReceiveSocket

qint64 UdpChatSocketManager::sendMessage(const QByteArray &data, const QHostAddress &targetAddress, quint16 targetPort)
{
    LOG_DEBUG("");

    if (!sendSocket) {
        qWarning() << "[UdpChatSocketManager] Send failed: sendSocket is null.";
        return -1;
    }

    lastSentData = data;
    lastSentTime = QDateTime::currentDateTimeUtc();

    const qint64 bytesWritten = sendSocket->writeDatagram(data, targetAddress, targetPort);

    if (bytesWritten == -1) {
        qWarning().nospace() << "[UdpChatSocketManager] writeDatagram failed: "
                             << sendSocket->errorString();
    }

    return bytesWritten;
}//sendMessage

void UdpChatSocketManager::cleanupSocket(QUdpSocket *&socket)
{
    LOG_DEBUG("");

    if (socket) {
        socket->close();
        delete socket;
        socket = nullptr;
    }
}//cleanupSocket

void UdpChatSocketManager::closeSockets()
{
    LOG_DEBUG("");

    cleanupSocket(recvSocket);
    cleanupSocket(sendSocket);
}//closeSockets

QByteArray UdpChatSocketManager::receiveDatagram(QHostAddress &sender, quint16 &port)
{
    LOG_DEBUG("");

    QByteArray datagram;
    datagram.resize(int(recvSocket->pendingDatagramSize()));
    recvSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port);
    return datagram;
}//receiveDatagram

bool UdpChatSocketManager::isSelfEcho(const QByteArray &datagram) const
{
    LOG_DEBUG("");

    const int echoSuppressionMs = 250;

    if (datagram != lastSentData)
        return false;

    const qint64 elapsed = lastSentTime.msecsTo(QDateTime::currentDateTimeUtc());
    return elapsed < echoSuppressionMs;
}//isSelfEcho

std::pair<QString, QString> UdpChatSocketManager::parseUserMessage(const QByteArray &raw) const
{
    LOG_DEBUG("");

    const QString fallbackText = QString::fromUtf8(raw);
    const QStringList parts = fallbackText.split(" - ", Qt::KeepEmptyParts);

    if (parts.size() >= 2) {
        const QString user = parts[0].trimmed();
        const QString message = parts.mid(1).join(" - ").trimmed();
        return { user, message };
    }

    return { "Unknown", fallbackText };
}//parseUserMessage

void UdpChatSocketManager::processPendingDatagrams()
{
    LOG_DEBUG("");

    if (!recvSocket)
        return;

    while (recvSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 senderPort;
        QByteArray datagram = receiveDatagram(sender, senderPort);

        if (isSelfEcho(datagram)) {
            lastSentData.clear();
            lastSentTime = {};
            continue;
        }

        const auto [user, message] = parseUserMessage(datagram);
        emit messageReceived(user, message);
    }
}//processPendingDatagrams

QString UdpChatSocketManager::lastError() const {
    LOG_DEBUG("");

    return sendSocket ? sendSocket->errorString() : "No socket";
}//lastError
