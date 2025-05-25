/*
 * Chester The Chat
 * Copyright (C) 2024 Timothy Millea
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

#include "UdpChatSocketManager.h"
#include "src/debugmacros.h"

#include <QUdpSocket>

bool UdpChatSocketManager::isConnected() const {
    LOG_DEBUG(Q_FUNC_INFO);

    return (sendSocket && sendSocket->state() == QAbstractSocket::BoundState) &&
           (recvSocket && recvSocket->state() == QAbstractSocket::BoundState);
}//isConnected

UdpChatSocketManager::UdpChatSocketManager(QObject *parent)
    : QObject(parent)
{
    LOG_DEBUG(Q_FUNC_INFO);

}//UdpChatSocketManager

UdpChatSocketManager::~UdpChatSocketManager()
{
    LOG_DEBUG(Q_FUNC_INFO);

    closeSockets();
}//UdpChatSocketManager

bool UdpChatSocketManager::bindSendSocket(const QHostAddress &localAddress)
{
    LOG_DEBUG(Q_FUNC_INFO);

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
    LOG_DEBUG(Q_FUNC_INFO);

    if (recvSocket) {
        recvSocket->close();
        delete recvSocket;
        recvSocket = nullptr;
    }
}//cleanupReceiveSocket

bool UdpChatSocketManager::createAndBindReceiveSocket(const QHostAddress &localAddress, quint16 port)
{
    LOG_DEBUG(Q_FUNC_INFO);

    recvSocket = new QUdpSocket(this);

    const bool success = recvSocket->bind(localAddress, port,
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

void UdpChatSocketManager::joinMulticastGroupSafely(const QHostAddress &groupAddress)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!recvSocket->joinMulticastGroup(groupAddress)) {
        qWarning().nospace() << "[UdpChatSocketManager] Failed to join multicast group "
                             << groupAddress.toString() << ": " << recvSocket->errorString();
    }
}//joinMulticastGroupSafely

bool UdpChatSocketManager::bindReceiveSocket(const QHostAddress &localAddress,
                                             const QHostAddress &groupAddress,
                                             quint16 port)
{
    LOG_DEBUG(Q_FUNC_INFO);

    cleanupReceiveSocket();

    if (!createAndBindReceiveSocket(localAddress, port))
        return false;

    if (groupAddress.isMulticast())
        joinMulticastGroupSafely(groupAddress);

    connect(recvSocket, &QUdpSocket::readyRead,
            this, &UdpChatSocketManager::processPendingDatagrams);

    return true;
}//bindReceiveSocket

qint64 UdpChatSocketManager::sendMessage(const QByteArray &data, const QHostAddress &targetAddress, quint16 targetPort)
{
    LOG_DEBUG(Q_FUNC_INFO);

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
    LOG_DEBUG(Q_FUNC_INFO);

    if (socket) {
        socket->close();
        delete socket;
        socket = nullptr;
    }
}//cleanupSocket

void UdpChatSocketManager::closeSockets()
{
    LOG_DEBUG(Q_FUNC_INFO);

    cleanupSocket(recvSocket);
    cleanupSocket(sendSocket);
}//closeSockets

void UdpChatSocketManager::setLoopbackMode(bool enabled)
{
    LOG_DEBUG(Q_FUNC_INFO);

    loopbackEnabled = enabled;
}//setLoopbackMode

void UdpChatSocketManager::setMulticastMode(bool enabled)
{
    LOG_DEBUG(Q_FUNC_INFO);

    multicastEnabled = enabled;
}//setMulticastMode

QByteArray UdpChatSocketManager::receiveDatagram(QHostAddress &sender, quint16 &port)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QByteArray datagram;
    datagram.resize(int(recvSocket->pendingDatagramSize()));
    recvSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port);
    return datagram;
}//receiveDatagram

bool UdpChatSocketManager::isSelfEcho(const QByteArray &datagram) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    const int echoSuppressionMs = 250;

    if (datagram != lastSentData)
        return false;

    const qint64 elapsed = lastSentTime.msecsTo(QDateTime::currentDateTimeUtc());
    return elapsed < echoSuppressionMs;
}//isSelfEcho

std::pair<QString, QString> UdpChatSocketManager::parseUserMessage(const QByteArray &raw) const
{
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
    LOG_DEBUG(Q_FUNC_INFO);

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

        // const QString messageText = QString::fromUtf8(datagram);
        const auto [user, message] = parseUserMessage(datagram);
        emit messageReceived(user, message);
    }
}//processPendingDatagrams

QString UdpChatSocketManager::lastError() const {
    LOG_DEBUG(Q_FUNC_INFO);

    return sendSocket ? sendSocket->errorString() : "No socket";
}//lastError
